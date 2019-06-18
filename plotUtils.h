#ifndef PLOTUTILS_H_INCLUDED
#define PLOTUTILS_H_INCLUDED

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TString.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TLine.h"
#include "TPaveStats.h"
#include "TPaveText.h"

namespace plotUtils
{
  void buildSigHisto(TFile *file, TTree *&sigTree, TH1F *sigHisto);
  void buildBkgHisto(TFile *file, TTree *&bkgTree, TH1F *bkgHisto);
  void buildSigHisto(TFile *file, TTree *sigTree, TH1F *sigHisto, Int_t *zeroEntries);
  void buildBkgHisto(TFile *file, TTree *bkgTree, TH1F *bkgHisto, Int_t *zeroEntries);
  void dataFillPt(TFile *file, TTree *t_nominal, TBranch *b_ljet_pt, TH1F *pt);
  void fillHisto(TTree *tree, TH1F *histo, TString chiBranch, TString weightBranch);
  void fillHisto(TTree *tree, TH1F *histo, TString chiBranch, TString weightBranch, Int_t &zeroEntries);
  void fillAxes(TH1F *sigHisto, TH1F *bkgHisto, Double_t xVals[], Double_t yVals[]);
  void findAtt(Double_t xVals[], Double_t yVals[], Int_t size, Double_t *bkg50Sig, Double_t *bkg80Sig);
  TString findBranchName(TTree *Tree, TString searchTerm);
  void getTree(TFile *file, TTree *&tree, TString searchTerm);
  void usage();
}
#endif //PLOTUTILS_H_INCLUDED
