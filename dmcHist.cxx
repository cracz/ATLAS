///////////////////////////////////////////////////////////////////////////
// This program takes  data, signal, or background files and produces
//  separate Pt, Eta, Phi, and Mass histograms. This is done for the
//  large jets and the small jets (Mass is only large jets). The data,
//  signal, and background files are passed to the program through
//  text files. The text files contain the full path to every individual
//  file of their certain type, so the program reads the file paths line
//  by line, stores the file paths in a vector, and uses that to access
//  each file. When the histograms are made, they are saved to root files.
//
//  !!!Make sure the "inputDir" variable is the path to the directory that
//  contains the text files!!!
//
//  This program only handles one case at a time, so it has to be used 
//  separately for data, signal, and background.
//
//  Execute the program with no arguments to show a usage statement.
///////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <sstream>
#include "TROOT.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TreeConnector.h"

using namespace std;

void usage();


int main(int argc, char* argv[])
{
  if(argc != 2) { usage(); return 1; }

  gROOT->ProcessLine("#include <vector>"); //Problems occur with the branches of vector<float> without this line

  string sampleName(argv[1]);                                    //data is "data_15_16.txt", signal is "ttbar.txt", background is "background.txt";
  string sampleNoExt(sampleName.substr(0, sampleName.find_last_of(".")));
  string inputDir = "/afs/cern.ch/user/c/cracz/work/DMC/input/";
 

  cout << "Accessing text file..." << endl << endl;
  ifstream str((inputDir+sampleName).c_str());
  if(str.fail()) { cout << sampleName << " could not be opened!" << endl; return 1; }


  cout << "Retrieving root file paths..." << endl << endl;
  string temp;
  vector<TString> files;   //Vector of file paths for the samples
  while(getline(str, temp)) 
    {
      while(temp.size() == 0) { getline(str, temp); }     // This is to skip blank lines

      files.push_back((TString)temp);
    }
  if (files.size() == 0) { cout << "The text file was empty!" << endl; return 1; }

  const int n_ljet_hists = 2;            //Number of large jet histograms (0 is leading jet, 1 is second leading, etc.)
  const int n_jet_hists = 3;             //Number of small jet histograms
  int nbins = 100;                     //Number of bins

  vector<TH1F*> h_ljet_pt(n_ljet_hists), h_ljet_eta(n_ljet_hists), h_ljet_phi(n_ljet_hists), h_ljet_m(n_ljet_hists);
  vector<TH1F*> h_jet_pt(n_jet_hists), h_jet_eta(n_jet_hists), h_jet_phi(n_jet_hists);

  stringstream ss;

  for(int i=0; i < n_ljet_hists; ++i)       //Make the empty histograms and number them (using the stringstream)
    {
      ss<<i;
      string jnum = ss.str();

      h_ljet_pt[i]  = new TH1F((string("h_ljet_pt")+jnum).c_str(),  (string("Large Jet Pt[")+jnum+string("]")).c_str(),  nbins, 0, 2000000);
      h_ljet_eta[i] = new TH1F((string("h_ljet_eta")+jnum).c_str(), (string("Large Jet Eta[")+jnum+string("]")).c_str(), nbins, -3, 3);
      h_ljet_phi[i] = new TH1F((string("h_ljet_phi")+jnum).c_str(), (string("Large Jet Phi[")+jnum+string("]")).c_str(), nbins, -4, 4);
      h_ljet_m[i]   = new TH1F((string("h_ljet_m")+jnum).c_str(),   (string("Large Jet Mass[")+jnum+string("]")).c_str(), nbins, 0, 250000);
      ss.str(string());     //Clears the stringstream
    }
      
  for(int j=0; j < n_jet_hists; ++j)
    {
      ss<<j;
      string jnum = ss.str();

      h_jet_pt[j]  = new TH1F((string("h_jet_pt")+jnum).c_str(),  (string("Pt[")+jnum+string("]")).c_str(),  nbins, 0, 2000000);
      h_jet_eta[j] = new TH1F((string("h_jet_eta")+jnum).c_str(), (string("Eta[")+jnum+string("]")).c_str(), nbins, -3, 3);
      h_jet_phi[j] = new TH1F((string("h_jet_phi")+jnum).c_str(), (string("Phi[")+jnum+string("]")).c_str(),  nbins, -4, 4);
      ss.str(string());     //Clears the stringstream
    }



  TFile *f = 0;  
  TTree *tree = 0;
  TreeConnector tc;
  
  cout << "Accessing files and filling histograms...";

  for(int i = 0; i < files.size(); ++i)
    {
      if(gSystem->AccessPathName(files[i])) 
	{ 
	  cout << "File " << i+1 << " could not be found!" << endl
	       << files[i] << endl; 
	  return 1; 
	}

      f = TFile::Open(files[i], "READ");

      if (f->GetSize() < 1) { f->Close(); continue; }                             //Skip the file if it is empty or can't be read

      tc.getTree(f, tree, "nominal"); 
      if (!tree) { std::cout << "Nominal tree not found!" << std::endl; return 1; }
      if (tree->GetEntries() == 0) { continue; }                                 //Skip the file if there are no entries
      
      if (files[i].Contains("data", TString::kExact)) tc.setAsData();                //figure out if weight branches need to be initialized (data has no weights) {{Might need a better way of doing this rather than going by the file's name}}
      else tc.setAsMC();

      
      tc.init(tree);                                        //Initialize connections to the branches inside 'tree'


      Float_t totalWeight = 1.0;
      Int_t nentries = tree->GetEntries();

      for (Long64_t j=0; j<nentries; ++j)
	{
	  tree->GetEntry(j);
	  
	  if(!tc.isData()) { totalWeight = tc.weight_mc*tc.weight_pileup*tc.weight_leptonSF*tc.weight_jvt; }
 

	  for(int nlj = 0; nlj < n_ljet_hists; ++nlj)
	    {
	      if(tc.ljet_pt->size() > nlj)
		{
		  h_ljet_pt[nlj]->Fill(tc.ljet_pt->at(nlj),   totalWeight);
		  h_ljet_eta[nlj]->Fill(tc.ljet_eta->at(nlj), totalWeight);
		  h_ljet_phi[nlj]->Fill(tc.ljet_phi->at(nlj), totalWeight);
		  h_ljet_m[nlj]->Fill(tc.ljet_m->at(nlj),     totalWeight);
		}
	    }


	  for(int nj = 0; nj < n_jet_hists; ++nj)
	    {
	      if(tc.jet_pt->size() > nj)
		{
		  h_jet_pt[nj]->Fill(tc.jet_pt->at(nj),   totalWeight);
		  h_jet_eta[nj]->Fill(tc.jet_eta->at(nj), totalWeight);
		  h_jet_phi[nj]->Fill(tc.jet_phi->at(nj), totalWeight);
		}
	    }
	  
	}

      f->Close();
    }


  cout << "done" << endl << endl;


  //SAVE ROOT FILES
  cout << "Saving histograms in " << sampleNoExt << ".root ..." << endl;


  TString newFileName(sampleNoExt+".root");
  TFile *h_file = TFile::Open(newFileName, "RECREATE"); h_file->cd();

  for(int i=0; i < n_ljet_hists; ++i)
    {
      h_ljet_pt[i]->Write(h_ljet_pt[i]->GetName());
      h_ljet_eta[i]->Write(h_ljet_eta[i]->GetName());
      h_ljet_phi[i]->Write(h_ljet_phi[i]->GetName());
      h_ljet_m[i]->Write(h_ljet_m[i]->GetName());
    }

  for(int j=0; j < n_jet_hists; ++j)
    {
      h_jet_pt[j]->Write(h_jet_pt[j]->GetName());
      h_jet_eta[j]->Write(h_jet_eta[j]->GetName());
      h_jet_phi[j]->Write(h_jet_phi[j]->GetName());
    }

  h_file->Close();


  cout << "Finished" << endl;
  return 0;
}//End main


void usage()
{
  cout << "Usage: dmcHist [textFileName]" << endl << endl
       << "The text file should be one that contains the full "
       << "path and file name to every file of a certain type "
       << "(data, signal, background), with each on a separate line."
       << endl;

}//End method: usage
