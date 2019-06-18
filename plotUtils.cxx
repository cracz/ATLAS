#include "plotUtils.h"
#include <cmath>
#include <iostream>
#include "TStyle.h"
#include "TSystem.h"
#include "TLatex.h"

namespace plotUtils
{
  /*
    Builds signal histogram
  */
  void buildSigHisto(TFile *file, TTree *&sigTree, TH1F *sigHisto)
  {
    getTree(file, sigTree, "Prime");
    
    TString sigChiBranch = findBranchName(sigTree, "chi");
    TString sigWeightBranch = findBranchName(sigTree, "Weight");
    
    fillHisto(sigTree, sigHisto, sigChiBranch, sigWeightBranch);
  }//End method: buildSigHisto
  
  
  /*
    Builds background histogram
  */
  void buildBkgHisto(TFile *file, TTree *&bkgTree, TH1F *bkgHisto)
  {
    getTree(file, bkgTree, "Dijet");
    
    TString bkgChiBranch = findBranchName(bkgTree, "chi");
    TString bkgWeightBranch = findBranchName(bkgTree, "Weight");
    
    fillHisto(bkgTree, bkgHisto, bkgChiBranch, bkgWeightBranch);
  }//End method: buildBkgHisto
  
  
  /*
    Builds signal histogram and finds number of zero entries
  */
  void buildSigHisto(TFile *file, TTree *sigTree, TH1F *sigHisto, Int_t *zeroEntries)
  {
    //                           main title               x axis                 y axis
    sigHisto->SetTitle("Distribution of Signal t Events; log #chi; Fraction of Events (x#bf{10^{-2}})");
    sigHisto->SetName("Signal");
    sigHisto->GetYaxis()->SetTitleOffset(1.3);
    
    getTree(file, sigTree, "Prime");
    
    TString chiBranch = findBranchName(sigTree, "chi");
    TString weightBranch = findBranchName(sigTree, "Weight");
    
    fillHisto(sigTree, sigHisto, chiBranch, weightBranch, *zeroEntries);  //Fill the histogram
  }//End overloaded method: buildSigHisto
  
  
  /*
    Builds background histogram and finds number of zero entries
  */
  void buildBkgHisto(TFile *file, TTree *bkgTree, TH1F *bkgHisto, Int_t *zeroEntries)
  {
    //                           main title                 x axis                 y axis
    bkgHisto->SetTitle("Distribution of Background Events; log #chi; Fraction of Events (x#bf{10^{-2}})");
    bkgHisto->SetName("Background");
    bkgHisto->GetYaxis()->SetTitleOffset(1.3);
  
    getTree(file, bkgTree, "Dijet");
  
    TString chiBranch = findBranchName(bkgTree, "chi");
    TString weightBranch = findBranchName(bkgTree, "Weight");
  
    fillHisto(bkgTree, bkgHisto, chiBranch, weightBranch, *zeroEntries);  //Fill the histogram
  }//End overloaded method: buildBkgHisto


  /*
    Fills the Pt histogram for data
  */
  void dataFillPt(TFile *file, TTree *tree, TBranch *b_ljet_pt, TH1F *pt)
  {
    getTree(file, tree, "nominal"); if(!tree) { std::cout << "Nominal tree not found!" << std::endl; return; }
    b_ljet_pt = tree->GetBranch("ljet_pt"); if(!b_ljet_pt) { std::cout << "ljet_pt branch not found!" << std::endl; return; }

    float pt_value;
    tree->SetBranchAddress("ljet_pt", &pt_value);

    for (int i=0; i<b_ljet_pt->GetEntries(); ++i)
      {
	tree->GetEntry(i);
	pt->Fill(pt_value);
      }
  }//End method: dataFillPt
  
  
  /*
    Fills an array with the coordinates of a ROC curve, the
    axis it fills depends on the option given, either "x" or "y".
    It sums the given histogram starting at the initial bin and 
    continues to the last bin, which has been found already and is
    determined by the size of the array that will hold the axis values.
  */
  void fillAxes(TH1F *sigHisto, TH1F *bkgHisto, Double_t xVals[], Double_t yVals[])
  {
    Int_t cutBin = 1;   //Set to the minimum initially, then incremented
    Int_t maxBin = sigHisto->GetNbinsX()+1;
    Int_t totalBins = maxBin - cutBin;
    
    Double_t sigTotIntegral = sigHisto->Integral(0, sigHisto->GetNbinsX()+1);
    Double_t bkgTotIntegral = bkgHisto->Integral(0, bkgHisto->GetNbinsX()+1);
    
    Double_t bkgIntFrac = 0.0;
    
    for (Int_t i = 0; i < totalBins; ++i)
      {
	xVals[i] = sigHisto->Integral(cutBin, maxBin) / sigTotIntegral;
	
	bkgIntFrac = bkgHisto->Integral(cutBin, maxBin) / bkgTotIntegral;      
	yVals[i] = 1/bkgIntFrac;
	
	++cutBin;
      }
  }//End method: fillAxes
  
  
  /*
    Creates weighted histograms
  */
  void fillHisto(TTree *tree, TH1F *histo, TString chiBranch, TString weightBranch)
  {
    Double_t chi, weight;
    long double totalWeight = 0.0;
    tree->SetBranchAddress(chiBranch, &chi);
    tree->SetBranchAddress(weightBranch, &weight);
    
    for (Double_t i; i < tree->GetEntries(); ++i)
      {
	tree->GetEntry(i);
	totalWeight += weight;
	histo->Fill(log(chi), weight);
      }
    
    histo->Scale(1.0/totalWeight);
  }//End method: fillHisto
  
  
  /*
    Creates weighted histograms and finds the number of entries
    with a value that is out of range
  */
  void fillHisto(TTree *tree, TH1F *histo, TString chiBranch, TString weightBranch, Int_t &zeroEntries)
  {
    Double_t chi, weight;
    long double totalWeight = 0.0;
    tree->SetBranchAddress(chiBranch, &chi);
    tree->SetBranchAddress(weightBranch, &weight);

    for (Double_t i; i < tree->GetEntries(); ++i)
      {
	tree->GetEntry(i);
	totalWeight += weight;
	histo->Fill(log(chi), weight);

	if (chi < 1e-15)        //Omit chi values out of range
	  zeroEntries++;
      }

    histo->Scale(100/totalWeight);    //100 is to remove leading zeros
  }//End overloaded method: fillHisto


  /*
    Find attributes of ROC curves
  */
  void findAtt(Double_t xVals[], Double_t yVals[], Int_t size, Double_t *bkg50Sig, Double_t *bkg80Sig)
  {
    Double_t diff50 = 0.0, minDiff50 = 1.0, diff80 = 0.0, minDiff80 = 1.0;     //min difference can't be 1, just a starting spot
    Int_t minDiff50Index = 0, minDiff80Index = 0;
    
    for (int i = 0; i < size; ++i)
      {
	diff50 = xVals[i] - 0.5; if (diff50 < 0.0) diff50 *= -1;
	diff80 = xVals[i] - 0.8; if (diff80 < 0.0) diff80 *= -1;
	
	if (diff50 < minDiff50) { minDiff50 = diff50; minDiff50Index = i; }
	if (diff80 < minDiff80) { minDiff80 = diff80; minDiff80Index = i; }
      }
    
    *bkg50Sig = yVals[minDiff50Index];
    *bkg80Sig = yVals[minDiff80Index];
  }//End method: findAtt
  
  
  
  /*
    Returns the name of the branch that contains the searchTerm
  */
  TString findBranchName(TTree *tree, TString searchTerm) 
  {
    TString branchName = "";
    
    for(int i = 0; i < 10 && i < tree->GetListOfBranches()->GetSize(); ++i) 
      {
	branchName = tree->GetListOfBranches()->At(i)->GetName();
	if(branchName.Contains(searchTerm)) break;
      }
    
    if (!branchName.Contains(searchTerm))
      {
	std::cout << "ABORTING ACTION" << std::endl
		  << "Error finding branch name which contains the term " << searchTerm 
		  << std::endl;
	exit(0);
      }
    
    return branchName;
  }//End method: findBranchName
  
  
  
  /*
    Returns a pointer to the tree with a name that contains the searchTerm
  */
  void getTree(TFile *file, TTree *&tree, TString searchTerm)
  {
    TString branchName = "";
    
    for (int i = 0; i < file->GetListOfKeys()->GetSize(); ++i)
      {
	branchName = file->GetListOfKeys()->At(i)->GetName();
	
	if (branchName.Contains(searchTerm)) 
	  file->GetObject(branchName, tree);
      }
    
    if (!tree) //Without this error check, the tree assignment doesn't work ??
      {
	std::cout << "ABORTING ACTION" << std::endl
		  << "The tree was not found in the file!" << std::endl;
	exit(0); 
      }
  }//End method: getTree


  /*
    Prints usage statement in the event of a starting error
  */
  void usage()
  {
    std::cout << std::endl << "Usage: rebuildHisto [sig||bkg||over||roc] [fileName]" << std::endl << std::endl
	      << "This program builds plots based on 2 arguments.  " << std::endl
	      << "The first argument specifies what type of plot is to be made, and the second specifies the file.  " << std::endl
	      << "The file should be an output file from Shower Deconstruction." << std::endl;
    exit(0);
  }//End method: usage
}
