///////////////////////////////////////////////////////////////////////////
// This program takes the output files of the dmcHist.cxx program and 
//  combines all of the histograms of one type into a THStack. This final 
//  histogram is then saved as a png image.
//
// The histogram files are passed to the program in the same way that
//  input files are passed to the dmcHist.cxx program. The full path
//  names of the histogram files are stored in the text file 
//  "hist_output.txt", and this program reads the text file to access
//  the histogram files. 
//
// Currently this program automatically accesses the "hist_output.txt"
//  file, so all the user has to do is specify which histogram
//  needs to be made.
//
//  !!!Make sure the "inputDir" variable is the path to the directory that
//  contains the text files!!!
///////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "TSystem.h"
#include "TROOT.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1F.h"
#include "THStack.h"
#include "TString.h"


using namespace std;

void usage();

int main(int argc, char* argv[])
{
  if (argc != 2) { usage(); return 1; }

  map<TString, TString> title;                        //maps are used to specify titles and colors
  title["h_ljet_pt0"]  = "First Large Jet p_{T}";
  title["h_ljet_eta0"] = "First Large Jet #eta";
  title["h_ljet_phi0"] = "First Large Jet #phi";
  title["h_ljet_m0"]   = "First Large Jet Mass";
  title["h_ljet_pt1"]  = "Second Large Jet p_{T}";
  title["h_ljet_eta1"] = "Second Large Jet #eta";
  title["h_ljet_phi1"] = "Second Large Jet #phi";
  title["h_ljet_m1"]   = "Second Large Jet Mass";
  title["h_jet_pt0"]   = "First Small Jet p_{T}";
  title["h_jet_eta0"]  = "First Small Jet #eta";
  title["h_jet_phi0"]  = "First Small Jet #phi";
  title["h_jet_pt1"]   = "Second Small Jet p_{T}";
  title["h_jet_eta1"]  = "Second Small Jet #eta";
  title["h_jet_phi1"]  = "Second Small Jet #phi";
  title["h_jet_pt2"]   = "Third Small Jet p_{T}";
  title["h_jet_eta2"]  = "Third Small Jet #eta";
  title["h_jet_phi2"]  = "Third Small Jet #phi";

  map<TString, TString> group;
  group["ttbar"] = "Signal";
  group["background"] = "Background";
  group["data"]  = "Data";

  map<TString, Int_t> color;
  color["ttbar"] = 44;
  color["background"] = 38;
  color["data"]  = 1;
  


  TString histName(argv[1]);
  //string sampleName(argv[2]);
  //string sampleNoExt(sampleName.substr(0, sampleName.find_last_of(".")));

  static const TString arr[] = {"h_ljet_pt0", "h_ljet_pt1", "h_ljet_eta0", "h_ljet_eta1", "h_ljet_phi0", "h_ljet_phi1", "h_ljet_m0", "h_ljet_m1", 
				"h_jet_pt0", "h_jet_pt1", "h_jet_pt2", "h_jet_eta0", "h_jet_eta1", "h_jet_eta2", "h_jet_phi0", "h_jet_phi1", "h_jet_phi2"};
  vector<TString> names (arr, arr + sizeof(arr) / sizeof(arr[0]) );

  bool nameFound = false;
  for(int i=0; i<names.size(); ++i)
    { 
      if (histName == names[i]) { nameFound = true; break; } 
    }
  if (nameFound == false) 
    { cout << "Make sure the histogram's name is entered correctly!" << endl; usage(); return 1; }


  cout << "Accessing text file..." << endl << endl;
  string inputDir = "/afs/cern.ch/user/c/cracz/work/DMC/input/";    //INPUT DIRECTORY
  string sampleName = "hist_output.txt";                          //INPUT FILE
  ifstream str((inputDir+sampleName).c_str()); 
  if(str.fail()) 
    { cout << sampleName << " could not be opened!" << endl; return 1; }


  cout << "Retrieving root file paths..." << endl << endl;
  string temp;
  vector<TString> files;      //Vector of files
  while(getline(str, temp)) 
    { files.push_back((TString)temp); }    //fill vectors with paths to files
  if (files.size() == 0)
    { cout << "The text file was empty!" << endl; return 1; }


  TFile *f = 0;
  TH1F *hist = 0;     //Used to transfer the histograms from files to the stack
  TH1F *hdata = 0;    //Used to get the data histogram
  THStack *stack  = new THStack("stack",  "Stack");
  TCanvas *canvas  = new TCanvas("canvas", "Title", 900, 700);
  TLegend *legend = new TLegend(.65, .65, .9, .9);


  cout << "Accessing files and printing histograms...";

  for (int i = 0; i < files.size(); ++i)
    {
      if (gSystem->AccessPathName(files[i])) { cout << "File " << i+1 << " could not be found!" << endl; return 1; }
      
      Ssiz_t lastSlash = files[i].Last('/') + 1;
      Ssiz_t length = files[i].Index(".", 1, lastSlash, TString::kExact) - lastSlash;
      TString groupName(files[i](lastSlash, length));    //Extract the group name from the file name (ttbar, background, or data)


      f = TFile::Open(files[i], "READ");      

      gROOT->cd();    /****This is so the histo doesn't die when each file is closed****/

      hist = (TH1F*)f->Get(histName)->Clone();

      if (groupName == "data") 
	{ 
	  hist->SetLineColor(color[groupName]); 
	  hist->SetMarkerStyle(kFullSquare);
	  hdata = hist;                                 //Get the data so it can be drawn last and separate from the stack
	  legend->AddEntry(hist, group[groupName]);
	  f->Close();
	  continue;
	}
      else
	{
	  hist->SetFillColor(color[groupName]);
	  hist->SetLineWidth(2);
	  legend->AddEntry(hist, group[groupName]);
	  stack->Add(hist);
	}

      if (i == 1) stack->SetTitle(title[histName]);    //Just to set the title only once

      f->Close();
    }
  cout << "done" << endl;

  stack->Draw(); canvas->Update();
  hdata->Draw("E1same"); canvas->Update();       //Draw data last so it's on top
  legend->Draw(); canvas->Update();

  if (histName.Contains("_pt") || histName.Contains("_m")) canvas->SetLogy();  //Log scale for Pt and Mass

  TString imageName(histName+".png");
  canvas->SaveAs(imageName);

  delete stack;
  delete canvas;
  return 0;
}//End main method


void usage()
{
  cout << "Usage: dsbPrint [histo_Name]" << endl << endl
       << "Accepted histogram names: " << endl 
       << "h_ljet_pt0, h_ljet_pt1" << endl
       << "h_ljet_eta0, h_ljet_eta1" << endl
       << "h_ljet_phi0, h_ljet_phi1" << endl
       << "h_ljet_m0, h_ljet_m1" << endl
       << "h_jet_pt0, h_jet_pt1, h_jet_pt2" << endl
       << "h_jet_eta0, h_jet_eta1, h_jet_eta2" << endl
       << "h_jet_phi0, h_jet_phi1, h_jet_phi2" << endl;
}
