#include "TFile.h"
#include "TH1.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TKey.h"
#include "TCollection.h"
#include "TStyle.h"
#include "TString.h"

#include <iostream>
#include <string>
#include <map>
#include <iterator>

struct Hist{
  
  //data type(0-data, 1-sig, 2-background)
  string name;
  int data_type;
  TFile* file;
  int color;
  vector<string> hist_names;
  map<string, TH1*> histograms;

  //Setup the new hist
  void init(string name_in, int data_type_in, TFile* file_in, vector<string> hist_names_in, int color_in){
    this->name = name_in;
    this->data_type = data_type_in;
    this->file = file_in;
    this->hist_names = hist_names_in;
    this->color = color_in;
    for(vector<string>::iterator it=hist_names_in.begin(); it!=hist_names_in.end(); ++it){
      this->histograms[*it] = (TH1*) this->file->Get((*it).c_str());
    }
  }
};

//Declare Later Functions
bool check_input(int sig_param, char set_param);
void plot_stack(vector<Hist> myHist, vector<Hist> order, string save_dir, int sig_param);
void plot_2D(vector<Hist> myHist, string save_dir, int sig_param);
void plot_flags(vector<Hist> myHist, string save_dir, int sig_param);
vector<string> make_stack_hist_names(string keyFilePath);
vector<string> make_2D_hist_names(string keyFilePath);
vector<string> make_nostack_hist_names(string keyFilePath);
vector<string> make_flag_hist_names(string keyFilePath);
TH1* make_sb_hist(TH1* data, TH1* scaledMC);
TH1* combine_MC(vector<Hist> myHists, string key);
TH1* combine_backgrounds(vector<Hist> myHist, string key);
//TH2* combine_backgrounds_2D(vector<Hist> myHist, string key);
int find_max(vector<Hist> myHist, TH1* tot_back, string key);
double calc_SF_ttbar(TH1* data, TH1* signal);
double calc_SF_bkg(TH1* data, TH1* bkg);
THStack* make_stack(string key, vector<Hist> order);



void make_plots(int sig_param = 0, char set_param = 's'){

  //sig_param determines which signal to use
  //  0 = nominal (default)
  //  1 = syst_410001
  //  2 = syst_410002
  //  3 = syst_410003
  //  4 = syst_410004


  //set_param determines which set of histograms to make
  //  s = stack histograms (default)
  //  t = 2D histograms
  //  n = "NOSTACK" histograms (those that shouldn't be stacked)
  //  f = "FLAG" histograms (histograms of dR sections)

  //########################################################### FILE & DIRECTORY CONTROLS
  string target_dir = "Output_1125/";
  string save_dir   = "Plots_1125/";
  string hist_dir   = "~/work/Shower_Decon/Histograms/";

  string prefix     = "ttbar_semileptonic_";
  string specs      = "_SD15GeVExcl_40GeVMt_20GeVMw_6max_Subjet5pcUp_Corr_";
  string correction = "TRUE";
  string ending     = "_PtWeighted_trackjet_1btin_output.root";

  string data_file      = prefix+"data"+specs+correction+ending;
  string diboson_file   = prefix+"diboson_ALL"+specs+correction+ending;
  string singletop_file = prefix+"singletop_ALL"+specs+correction+ending;
  string wjets_file     = prefix+"wjets_ALL"+specs+correction+ending;
  string zjets_file     = prefix+"zjets_ALL"+specs+correction+ending;
  string nominal_file   = prefix+"ttbar_410000"+specs+correction+ending;
  string syst_1_file    = prefix+"ttbar_syst_410001"+specs+correction+ending;
  string syst_2_file    = prefix+"ttbar_syst_410002"+specs+correction+ending;
  string syst_3_file    = prefix+"ttbar_syst_410003"+specs+correction+ending;
  string syst_4_file    = prefix+"ttbar_syst_410004"+specs+correction+ending;

  string keyFilePath = hist_dir+target_dir+prefix+"data"+specs+correction+ending;
  //#######################################################################

  //Check for bad input parameters
  bool good_input = check_input(sig_param, set_param);
  if(!good_input){
    cout << "Bad input parameters" << endl;
    return;
  }


  //Don't want statistics
  gStyle->SetOptStat(0);

  // //Make the arrays of hist_names
  vector<string> hist_names;
  if(set_param == 's') hist_names = make_stack_hist_names(keyFilePath);
  if(set_param == 't') hist_names = make_2D_hist_names(keyFilePath);
  //if(set_param == 'n') hist_names = make_nostack_hist_names(keyFilePath);
  if(set_param == 'f') hist_names = make_flag_hist_names(keyFilePath);

  //Make all the needed hist
  //Data
  Hist data;
  string name0 = "Data";
  int data_type0 = 0;
  TFile* file0 = new TFile((hist_dir+target_dir+data_file).c_str());
  int color0 = 1; //Black
  data.init(name0, data_type0, file0, hist_names, color0);

  //Background
  Hist diboson;
  string name1 = "Diboson";
  int data_type1 = 2;
  TFile* file1 = new TFile((hist_dir+target_dir+diboson_file).c_str());
  int color1 = 432-9; //Light cyan
  diboson.init(name1, data_type1, file1, hist_names, color1);

  Hist singletop;
  string name2 = "Singletop";
  int data_type2 = 2;
  TFile* file2 = new TFile((hist_dir+target_dir+singletop_file).c_str());
  int color2 = 600 - 7; //Light blue
  singletop.init(name2, data_type2, file2, hist_names, color2);

  Hist wjets;
  string name3 = "wjets";
  int data_type3 = 2;
  TFile* file3 = new TFile((hist_dir+target_dir+wjets_file).c_str());
  int color3 = 800 - 7; //Light orange
  wjets.init(name3, data_type3, file3, hist_names, color3);

  Hist zjets;
  string name4 = "zjets";
  int data_type4 = 2;
  TFile* file4 = new TFile((hist_dir+target_dir+zjets_file).c_str());
  int color4 = 1416+2; //Dark Green
  zjets.init(name4, data_type4, file4, hist_names, color4);

  //Signal
  Hist signal;
  int data_type5 = 1;
  int color5 = 632-7; //Light red
  TFile* file5 = 0;
  string name5;

  if(sig_param == 0){
    file5 = new TFile((hist_dir+target_dir+nominal_file).c_str());
    name5 = "ttbar - Nominal";
  }
  if(sig_param == 1){
    file5 = new TFile((hist_dir+target_dir+syst_1_file).c_str());
    name5 = "ttbar - Syst 410001";
  }
  if(sig_param == 2){
    file5 = new TFile((hist_dir+target_dir+syst_2_file).c_str());
    name5 = "ttbar - Syst 410002";
  }
  if(sig_param == 3){
    file5 = new TFile((hist_dir+target_dir+syst_3_file).c_str());
    name5 = "ttbar - Syst 410003";
  }
  if(sig_param == 4){
    file5 = new TFile((hist_dir+target_dir+syst_4_file).c_str());
    name5 = "ttbar - Syst 410004";
  }

  signal.init(name5, data_type5, file5, hist_names, color5);

  //Make a vector of hists
  vector<Hist> myHist;
  myHist.push_back(data);
  myHist.push_back(diboson);
  myHist.push_back(singletop);
  myHist.push_back(wjets);
  myHist.push_back(zjets);
  myHist.push_back(signal);

  //Make list of order for backgrounds in stack
  vector<Hist> order;
  order.push_back(diboson);
  order.push_back(singletop);
  order.push_back(wjets);
  order.push_back(zjets);
  order.push_back(signal);
  
  if(set_param == 's') plot_stack(myHist, order, save_dir, sig_param);
  if(set_param == 't') plot_2D(myHist, save_dir, sig_param);
  if(set_param == 'f') plot_flags(myHist, save_dir, sig_param);
}//End main (make_plots())



void plot_stack(vector<Hist> myHist, vector<Hist> order, string save_dir, int sig_param){

  //Get Keys
  vector<string> keys = myHist[0].hist_names;

  double SF_ttbar = 0.0;
  double SF_bkg   = 0.0;
  bool SF_found = false;

  for(vector<string>::iterator it = keys.begin(); it!=keys.end(); ++it){

    if(*it == "h_INTEGRAL") continue;

    cout << endl << *it << "..." << endl;

    //Get scale factors
    if(!SF_found){
      TH1* data_int = 0;
      TH1* signal_int = 0;
      for(uint i = 0; i<myHist.size(); ++i){
	if(myHist[i].data_type == 0) data_int   = (TH1*)myHist[i].histograms["h_INTEGRAL"]->Clone();
	if(myHist[i].data_type == 1) signal_int = (TH1*)myHist[i].histograms["h_INTEGRAL"]->Clone();
      }

      TH1 *tot_bkg_int = combine_backgrounds(myHist, "h_INTEGRAL");

      double purity_ttbar = 0.85;
      double purity_bkg = 0.15;
      double I_data = data_int->Integral();
      double I_ttbar = signal_int->Integral();
      double I_bkg = tot_bkg_int->Integral();

      SF_ttbar = purity_ttbar*I_data/I_ttbar;
      SF_bkg   = purity_bkg*I_data/I_bkg;
      SF_found = true;
    }


    //Get total MC
    TH1* tot_MC = combine_MC(myHist, *it);            //UNSCALED

    //Get signal and data histograms
    TH1 *signal = 0;
    TH1 *data = 0;

    for(uint i = 0; i<myHist.size(); ++i){
      if(myHist[i].data_type == 0) data   = (TH1*)myHist[i].histograms[*it]->Clone();
      if(myHist[i].data_type == 1) signal = (TH1*)myHist[i].histograms[*it]->Clone();
    }

    //Get total background
    TH1 *tot_bkg = combine_backgrounds(myHist, *it);      //ONLY BACKGROUND


    //SCALE SIGNAL AND BACKGROUND HISTOGRAMS
    vector<TH1*> order_vector;                                         //NEED TO CHANGE order TO A VECTOR FOR THE SCALING TO WORK
    for(uint i = 0; i<order.size(); ++i){
      order_vector.push_back(order[i].histograms[*it]);
    }
    for(uint i = 0; i<order_vector.size(); ++i){
      if(i < order_vector.size()-1)  order_vector[i]->Scale(SF_bkg);
      if(i == order_vector.size()-1) order_vector[i]->Scale(SF_ttbar);
    }

    tot_bkg->Scale(SF_bkg);
    signal->Scale(SF_ttbar);

    double sig_events = signal->Integral();
    double bkg_events = tot_bkg->Integral();
    cout << "Signal events: " << sig_events << endl
	 << "Background events: " << bkg_events << endl;


    TH1 *tot_scaled_MC = (TH1*)tot_bkg->Clone();
    tot_scaled_MC->Add(signal);                             //TOTAL SCALED MC     --     NEEDED FOR FINDING MAX HIST AND MAKING D/MC HIST


    //Make Stack Histogram
    THStack *stack = new THStack("background","background");
    
    for(uint i=0; i<order.size(); ++i){
      order_vector[i]->SetFillColor(order[i].color);
      order_vector[i]->SetLineColor(1);
      stack->Add(order_vector[i]);
    }

    //Make Legend
    TLegend* legend = new TLegend(0.65,0.55,0.85,.9);

    //Need to create a new TCanvas to draw on
    TCanvas* c1 = new TCanvas("c1","c1",700,600);
    c1->Divide(1,2);
    c1->cd(1);


    //Get max histogram
    int max_index = find_max(myHist, tot_scaled_MC, *it);

    //Draw max histogram
    if(max_index == -1){
      tot_scaled_MC->SetLineColor(kWhite);
      tot_scaled_MC->Draw();
    }else{
      myHist[max_index].histograms[*it]->SetLineColor(kWhite);
      myHist[max_index].histograms[*it]->Draw();
      myHist[max_index].histograms[*it]->SetLineColor(kBlack);
    }
    
    stack->Draw("samehist");

    //Fill Legend
    for (vector<Hist>::reverse_iterator backwards = order.rbegin(); backwards != order.rend(); ++backwards){
      legend->AddEntry(backwards->histograms[*it], (backwards->name).c_str(), "f");
    }


    //Draw data
    for(uint i = 0; i<myHist.size(); ++i){
      if(myHist[i].data_type == 0){
	//get wanted plot
	TH1* plot = myHist[i].histograms[*it];

	plot->SetLineColor(myHist[i].color);
	legend->AddEntry(plot, (myHist[i].name).c_str(), "lep");

	plot->Draw("same");
      }
    }
    
    legend->Draw();

    //Make D/MC
    c1->cd(2);
    string key = *it;
    TH1* s_b = make_sb_hist(data, tot_scaled_MC);


    s_b->SetTitle("");
    s_b->GetYaxis()->SetTitle("Data / SM");
    s_b->SetFillColor(0);
    s_b->SetStats(kFALSE);
    s_b->Draw("hist");

    string sig_type = to_string(sig_param);

    //Save histogram as png file
    string save_name = "./Plots/"+save_dir+*it+"_SIG_"+sig_type+".png";
    c1->SaveAs((save_name).c_str());

    
    //Save the signal histograms for later use when comparing different signals
    string sig_file_name = *it+"_SIG_"+sig_type+"_SIGNAL_ONLY.root";
    TFile* f = new TFile(sig_file_name.c_str(), "RECREATE");
    f->cd();
    signal->Write();
    f->Close();
    

    //Delete Canvas
    if(c1!=0){
      delete c1;
      c1 = 0;
    }

    //Delete Legend
    if(legend !=0){
      delete legend;
      legend = 0;
    }

  }
}//End function plot_stack()



void plot_2D(vector<Hist> myHist, string save_dir, int sig_param){

  //Get Keys
  vector<string> keys = myHist[0].hist_names;

  double SF_ttbar = 0.0;
  double SF_bkg   = 0.0;
  bool SF_found = false;

  gStyle->SetOptStat(0);
  
  for(vector<string>::iterator it = keys.begin(); it!=keys.end(); ++it){

    if(*it == "h_INTEGRAL") continue;

    cout << endl << *it << "..." << endl;

    //Get scale factors
    if(!SF_found){
      TH1* data_int = 0;
      TH1* signal_int = 0;
      for(uint i = 0; i<myHist.size(); ++i){
	if(myHist[i].data_type == 0) data_int   = (TH1*)myHist[i].histograms["h_INTEGRAL"]->Clone();
	if(myHist[i].data_type == 1) signal_int = (TH1*)myHist[i].histograms["h_INTEGRAL"]->Clone();
      }

      TH1 *tot_bkg_int = combine_backgrounds(myHist, "h_INTEGRAL");

      double purity_ttbar = 0.85;
      double purity_bkg = 0.15;
      double I_data = data_int->Integral();
      double I_ttbar = signal_int->Integral();
      double I_bkg = tot_bkg_int->Integral();

      SF_ttbar = purity_ttbar*I_data/I_ttbar;
      SF_bkg   = purity_bkg*I_data/I_bkg;
      SF_found = true;
    }

    
    TH1 *signal = 0;
    //TH1 *data = 0;
    TH1 *tot_bkg = combine_backgrounds(myHist, *it);

    for(uint i = 0; i<myHist.size(); ++i){
      //if(myHist[i].data_type == 0) data   = (TH1*)myHist[i].histograms[*it]->Clone();
      if(myHist[i].data_type == 1) signal = (TH1*)myHist[i].histograms[*it]->Clone();
    }

    //double SF_ttbar = calc_SF_ttbar(data,signal);
    //double SF_bkg   = calc_SF_bkg(data,tot_bkg);
    signal->Scale(SF_ttbar);
    tot_bkg->Scale(SF_bkg);

    double sig_events = signal->Integral();
    double bkg_events = tot_bkg->Integral();
    cout << "Signal events: " << sig_events << endl
	 << "Background events: " << bkg_events << endl;

    TCanvas* c1 = new TCanvas("c1","c1",700,600);
    TCanvas* c2 = new TCanvas("c2","c2",700,600);

    c1->cd();
    signal->GetYaxis()->SetTitleOffset(1.25);
    signal->Draw("COLZ");

    c2->cd();
    tot_bkg->GetYaxis()->SetTitleOffset(1.25);
    tot_bkg->Draw("COLZ");

    string sig_type = to_string(sig_param);

    string sig_save_name = "./Plots/"+save_dir+*it+"_SIG_"+sig_type+".png";
    c1->SaveAs(sig_save_name.c_str());

    string bkg_save_name = "./Plots/"+save_dir+*it+"_BKG.png";
    c2->SaveAs(bkg_save_name.c_str());

    if(c1!=0){
      delete c1;
      c1 = 0;
    }
    if(c2!=0){
      delete c2;
      c2 = 0;
    }
  }
}//End function plot_2D()



void plot_flags(vector<Hist> myHist, string save_dir, int sig_param){

  TCanvas* c1 = new TCanvas("c1","c1",700,600);
  TCanvas* c2 = new TCanvas("c2","c2",700,600);
  
  THStack* stack1 = new THStack();
  THStack* stack2 = new THStack();
  stack1->SetTitle("Nominal ttbar, Large jet p_{t} for sections of #DeltaR_{12}");
  stack2->SetTitle("Nominal ttbar, Large jet p_{t} for sections of #DeltaR_{12}");
  
  TLegend* leg1 = new TLegend(0.65,0.9,0.9,0.6);
  TLegend* leg2 = new TLegend(0.65,0.9,0.9,0.6);

  vector<string> keys = myHist[0].hist_names;
  vector<TH1*> h_vect;
  TH1* signal = 0;
  TH1* data = 0;

  //Scale each signal histogram and put them in a vector
  for(vector<string>::iterator it = keys.begin(); it!=keys.end(); ++it){
    
    for(uint i = 0; i<myHist.size(); ++i){
      if(myHist[i].data_type == 0) data   = (TH1*)myHist[i].histograms[*it]->Clone();
      if(myHist[i].data_type == 1) signal = (TH1*)myHist[i].histograms[*it]->Clone();
    }

    double SF_ttbar = calc_SF_ttbar(data,signal);
    signal->Scale(SF_ttbar);
    
    h_vect.push_back(signal);
  }

  //HARD CODED COLORS AND NUMBER OF HISTOGRAMS ---- MUST CHANGE IF THE "FLAG" HISTOGRAMS CHANGE
  for(int i=0; i<3; ++i){
    if(i==0) h_vect[i]->SetLineColor(kBlack);
    if(i==1) h_vect[i]->SetLineColor(kBlue-2);
    if(i==2) h_vect[i]->SetLineColor(kRed-2);

    h_vect[i]->SetLineWidth(3);

    stack1->Add(h_vect[i]);
    leg1->AddEntry(h_vect[i], h_vect[i]->GetTitle(), "l");
  }
  for(int j=3; j<6; ++j){
    if(j==3) h_vect[j]->SetLineColor(kGreen-2);
    if(j==4) h_vect[j]->SetLineColor(kMagenta-2);
    if(j==5) h_vect[j]->SetLineColor(kCyan-2);

    h_vect[j]->SetLineWidth(3);

    stack2->Add(h_vect[j]);
    leg2->AddEntry(h_vect[j], h_vect[j]->GetTitle(), "l");
  }

  c1->cd();
  stack1->Draw("nostackhist");
  stack1->GetXaxis()->SetTitle("p_{t} (GeV)");            //Have to draw first before changing axes, not sure why.
  stack1->GetYaxis()->SetTitle("Events");
  stack1->Draw("nostackhist");
  leg1->Draw();
  c1->Update();

  string sig_type = to_string(sig_param);

  string save_name1 = "./Plots/"+save_dir+"dR12_sections_1-3"+"_SIG_"+sig_type+".png";
  c1->SaveAs((save_name1).c_str());

  c2->cd();
  stack2->Draw("nostackhist");
  stack2->GetXaxis()->SetTitle("p_{t} (GeV)");
  stack2->GetYaxis()->SetTitle("Events");
  stack2->Draw("nostackhist");
  leg2->Draw();
  c2->Update();

  string save_name2 = "./Plots/"+save_dir+"dR12_sections_4-6"+"_SIG_"+sig_type+".png";
  c2->SaveAs((save_name2).c_str());

  
  //Delete Canvas
  if(c1!=0){
    delete c1;
    c1 = 0;
  }
  if(c2!=0){
    delete c2;
    c2 = 0;
  }

  //Delete Legend
  if(leg1 !=0){
    delete leg1;
    leg1 = 0;
  }
  if(leg2 !=0){
    delete leg2;
    leg2 = 0;
  }

}//End function plot_flags()



bool check_input(int sig_param, char set_param){

  bool good_input = false;
  vector<int>  good_sig_param = {0,1,2,3,4};
  vector<char> good_set_param = {'s','t','n','f'};
  int sig_possible = good_sig_param.size();
  int set_possible = good_set_param.size();
  bool good_sig_input = false;
  bool good_set_input = false;

  for(int i=0; i<sig_possible; ++i){
    if(sig_param == good_sig_param[i]){
      good_sig_input = true;
      break;
    }
  }
  for(int j=0; j<set_possible; ++j){
    if(set_param == good_set_param[j]){
      good_set_input = true;
      break;
    }
  }
  
  if(good_sig_input && good_set_input) good_input = true;
  
  return good_input;
}//End function check_input()



vector<string> make_stack_hist_names(string keyFilePath){
  TFile nameFile(keyFilePath.c_str());
  TIter next(nameFile.GetListOfKeys());
  TKey* histKey;
  TString title;
  vector<string> hist_names;

  while((histKey = (TKey*) next())){
    title = histKey->GetName();

    if(strcmp(histKey->GetClassName(), "TH1D") != 0) continue;
    if(title.Contains("FLAG")) continue;
    if(title.Contains("NOSTACK")) continue;

    string name = histKey->GetName();
    hist_names.push_back(name);
  }
  return hist_names;
}//End function make_stack_hist_names()



vector<string> make_2D_hist_names(string keyFilePath){
  TFile nameFile(keyFilePath.c_str());
  TIter next(nameFile.GetListOfKeys());
  TKey* histKey;
  vector<string> hist_names;

  while((histKey = (TKey*) next())){
    string name = histKey->GetName();
    TString class_name = histKey->GetClassName();

    if(class_name.CompareTo("TH2D") == 0 || name == "h_INTEGRAL")
      hist_names.push_back(name);
  }
  return hist_names;
}//End function make_2D_hist_names()



vector<string> make_flag_hist_names(string keyFilePath){
  TFile nameFile(keyFilePath.c_str());
  TIter next(nameFile.GetListOfKeys());
  TKey* histKey;
  vector<string> hist_names;

  while((histKey = (TKey*) next())){
    string name = histKey->GetName();
    TString t_name = histKey->GetName();

    if(t_name.Contains("FLAG"))
      hist_names.push_back(name);
  }
  return hist_names;  
}//End function make_flag_hist_names()



TH1* combine_MC(vector<Hist> myHist, string key){
  vector<TH1*> only_back;
  TH1* tot_back = new TH1F();
  for (uint i=0; i<myHist.size(); ++i){
    if(myHist[i].data_type == 1 || myHist[i].data_type == 2){
      only_back.push_back(myHist[i].histograms[key]);
    }
  }
  for(uint i=0; i<only_back.size(); ++i){
    if(i==0){
      tot_back = (TH1*) only_back[i]->Clone();
    }else{
      tot_back->Add(only_back[i]);
    }
  }
  return tot_back;
}//End function combine_MC()



TH1* combine_backgrounds(vector<Hist> myHist, string key){
  vector<TH1*> only_back;
  TH1* tot_back = new TH1F();
  for (uint i=0; i<myHist.size(); ++i){
    if(myHist[i].data_type == 2){
      only_back.push_back(myHist[i].histograms[key]);
    }
  }
  for(uint i=0; i<only_back.size(); ++i){
    if(i==0){
      tot_back = (TH1*) only_back[i]->Clone();
    }else{
      tot_back->Add(only_back[i]);
    }
  }
  return tot_back;
}//End function combine_backgrounds()



int find_max(vector<Hist> myHist, TH1* tot_back, string key){
  float current_max = tot_back->GetMaximum();
  int current_index = -1;
  for(uint i=0; i<myHist.size(); ++i){
    if(myHist[i].data_type == 0){
      if(myHist[i].histograms[key]->GetMaximum() > current_max){
	current_max = myHist[i].histograms[key]->GetMaximum();
	current_index = i;
      }
    }
  }
  return current_index;
}//End function find_max()



THStack* make_stack(string key, vector<Hist> order){

  THStack* stack = new THStack("background", "background");

  for(uint i=0; i<order.size(); ++i){
    order[i].histograms[key]->SetFillColor(order[i].color);
    order[i].histograms[key]->SetLineColor(1);
    stack->Add(order[i].histograms[key]);
  }
  return stack;
}//End function make_stack()



double calc_SF_ttbar(TH1* data, TH1* signal){
  double purity_ttbar = 0.85;
  double I_data  = data->Integral();
  double I_ttbar = signal->Integral();
  double SF = purity_ttbar*I_data/I_ttbar;
  return SF;
}//End function calc_SF_ttbar()



double calc_SF_bkg(TH1* data, TH1* bkg){
  double purity_bkg = 0.15;
  double I_data = data->Integral();
  double I_bkg  = bkg->Integral();
  double SF = purity_bkg*I_data/I_bkg;
  return SF;
}//End function calc_SF_bkg()



TH1* make_sb_hist(TH1* data, TH1* scaledMC){
  TH1* s_b = (TH1*)data->Clone();
  s_b->Divide(scaledMC);



  /*
  int nBins = data->GetNbinsX();
  TH1* s_b = (TH1*)data->Clone();

  for(int bin=0; bin < nBins; ++bin){
    
    float databin = data->GetBinContent(bin);
    float mcbin = scaledMC->GetBinContent(bin);
    
    if(mcbin == 0.0){
      s_b->SetBinContent(bin, -1);
    }else{
      s_b->SetBinContent(bin, databin/mcbin);
    }
  }
  */
  s_b->GetYaxis()->SetRangeUser(0,2);
  s_b->SetLineColor(1);
  return s_b;
}//End function make_sb_hist()
