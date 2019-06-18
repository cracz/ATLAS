//////
//This class is meant to automatically handle connections to multiple branches
//in data, signal, or background root files when all of the files are using the 
//same names for their branches. All that *should* be necessary is for you to
//make a TreeConnector object, check if the input file is data or not, and 
//then initialize the connections with init(TTree*). This way, it is easy to
//loop through many files and connect each time.
//////

#ifndef TREECONNECTOR_H
#define TREECONNECTOR_H

#include <iostream>
#include <vector>
#include "TTree.h"
#include "TFile.h"
using std::vector;


class TreeConnector
{
 private:
  bool fileIsData;

 public:
  void init(TTree *tree);
  void setAsData();
  void setAsMC();
  bool isData();
  void getTree(TFile *file, TTree *&tree, TString searchTerm);

  // Tree you are connecting to
  TTree *cTree;

  // Declaration of leaf types
  Float_t         weight_mc;
  Float_t         weight_pileup;
  Float_t         weight_leptonSF;
  Float_t         weight_bTagSF_70;
  Float_t         weight_trackjet_bTagSF_70;
  Float_t         weight_jvt;

  //vector<float>   *el_pt;
  //vector<float>   *el_eta;
  //vector<float>   *el_phi;
  //vector<float>   *mu_pt;
  //vector<float>   *mu_eta;
  //vector<float>   *mu_p

  vector<float>   *jet_pt;
  vector<float>   *jet_eta;
  vector<float>   *jet_phi;
  //vector<float>   *jet_mv2c00;
  //vector<float>   *jet_mv2c10;
  //vector<float>   *jet_mv2c20;
  //vector<float>   *jet_jvt;
  //vector<int>     *jet_truthflav;
  //vector<char>    *jet_isbtagged_70;

  vector<float>   *ljet_pt;
  vector<float>   *ljet_eta;
  vector<float>   *ljet_phi;
  vector<float>   *ljet_m;
  //vector<float>   *ljet_sd12;

  //Float_t         met_met;

  // List of branches
  TBranch        *b_weight_mc;   //!
  TBranch        *b_weight_pileup;   //!
  TBranch        *b_weight_leptonSF;   //!
  TBranch        *b_weight_bTagSF_70;   //!
  TBranch        *b_weight_trackjet_bTagSF_70;   //!
  TBranch        *b_weight_jvt;   //!

  /* TBranch        *b_el_pt;   //! */
  /* TBranch        *b_el_eta;   //! */
  /* TBranch        *b_el_phi;   //! */
  /* TBranch        *b_mu_pt;   //! */
  /* TBranch        *b_mu_eta;   //! */
  /* TBranch        *b_mu_phi;   //! */

  TBranch        *b_jet_pt;   //!
  TBranch        *b_jet_eta;   //!
  TBranch        *b_jet_phi;   //!
  /* TBranch        *b_jet_mv2c00;   //! */
  /* TBranch        *b_jet_mv2c10;   //! */
  /* TBranch        *b_jet_mv2c20;   //! */
  /* TBranch        *b_jet_jvt;   //! */
  /* TBranch        *b_jet_truthflav;   //! */
  /* TBranch        *b_jet_isbtagged_70;   //! */

  TBranch        *b_ljet_pt;   //!
  TBranch        *b_ljet_eta;   //!
  TBranch        *b_ljet_phi;   //!
  TBranch        *b_ljet_m;   //!
  TBranch        *b_ljet_sd12;   //!

  //TBranch        *b_met_met;   //!
};


/*
  Returns a pointer to the tree with a name that contains the searchTerm
*/
void TreeConnector::getTree(TFile *file, TTree *&tree, TString searchTerm)
{
  TString branchName = "";
    
  for (int i = 0; i < file->GetListOfKeys()->GetSize(); ++i)
    {
      branchName = file->GetListOfKeys()->At(i)->GetName();
	
      if (branchName.Contains(searchTerm)) 
	file->GetObject(branchName, tree);
    }
    
  if (!tree) //Keep this error check
    {
      std::cout << "ABORTING ACTION" << std::endl
		<< "The tree was not found in the file!" << std::endl;
      exit(0); 
    }
}

void TreeConnector::setAsData() { fileIsData = true; }

void TreeConnector::setAsMC() { fileIsData = false; }

bool TreeConnector::isData() { return fileIsData; }

void TreeConnector::init(TTree *tree)
{
  // Set object pointer
  //  el_pt = 0;
  //  el_eta = 0;
  //  el_phi = 0;
  //  mu_pt = 0;
  //  mu_eta = 0;
  //  mu_phi = 0;
  jet_pt = 0;
  jet_eta = 0;
  jet_phi = 0;
  // jet_e = 0;
  // jet_mv2c00 = 0;
  // jet_mv2c10 = 0;
  // jet_mv2c20 = 0;
  // jet_ip3dsv1 = 0;
  // jet_jvt = 0;
  // jet_truthflav = 0;
  // jet_isTrueHS = 0;
  // jet_isbtagged_70 = 0;
  ljet_pt = 0;
  ljet_eta = 0;
  ljet_phi = 0;
  // ljet_e = 0;
  ljet_m = 0;
  // ljet_sd12 = 0;


  // Set branch addresses and branch pointers
  if (!tree) return;

  cTree = tree;

  if (fileIsData == false)
    {
      cTree->SetBranchAddress("weight_mc", &weight_mc, &b_weight_mc);
      cTree->SetBranchAddress("weight_pileup", &weight_pileup, &b_weight_pileup);
      cTree->SetBranchAddress("weight_leptonSF", &weight_leptonSF, &b_weight_leptonSF);
      //cTree->SetBranchAddress("weight_bTagSF_70", &weight_bTagSF_70, &b_weight_bTagSF_70);
      //cTree->SetBranchAddress("weight_trackjet_bTagSF_70", &weight_trackjet_bTagSF_70, &b_weight_trackjet_bTagSF_70);
      cTree->SetBranchAddress("weight_jvt", &weight_jvt, &b_weight_jvt);
    }
  
  //  cTree->SetBranchAddress("el_pt", &el_pt, &b_el_pt);
  //  cTree->SetBranchAddress("el_eta", &el_eta, &b_el_eta);
  //  cTree->SetBranchAddress("el_phi", &el_phi, &b_el_phi);
  //  cTree->SetBranchAddress("mu_pt", &mu_pt, &b_mu_pt);
  //  cTree->SetBranchAddress("mu_eta", &mu_eta, &b_mu_eta);
  //  cTree->SetBranchAddress("mu_phi", &mu_phi, &b_mu_phi);

  cTree->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
  cTree->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
  cTree->SetBranchAddress("jet_phi", &jet_phi, &b_jet_phi);
  // cTree->SetBranchAddress("jet_e", &jet_e, &b_jet_e);
  // cTree->SetBranchAddress("jet_mv2c00", &jet_mv2c00, &b_jet_mv2c00);
  // cTree->SetBranchAddress("jet_mv2c10", &jet_mv2c10, &b_jet_mv2c10);
  // cTree->SetBranchAddress("jet_mv2c20", &jet_mv2c20, &b_jet_mv2c20);
  // cTree->SetBranchAddress("jet_ip3dsv1", &jet_ip3dsv1, &b_jet_ip3dsv1);
  // cTree->SetBranchAddress("jet_jvt", &jet_jvt, &b_jet_jvt);
  // cTree->SetBranchAddress("jet_truthflav", &jet_truthflav, &b_jet_truthflav);
  // cTree->SetBranchAddress("jet_isTrueHS", &jet_isTrueHS, &b_jet_isTrueHS);
  // cTree->SetBranchAddress("jet_isbtagged_70", &jet_isbtagged_70, &b_jet_isbtagged_70);
  cTree->SetBranchAddress("ljet_pt", &ljet_pt, &b_ljet_pt);
  cTree->SetBranchAddress("ljet_eta", &ljet_eta, &b_ljet_eta);
  cTree->SetBranchAddress("ljet_phi", &ljet_phi, &b_ljet_phi);
  // cTree->SetBranchAddress("ljet_e", &ljet_e, &b_ljet_e);
  cTree->SetBranchAddress("ljet_m", &ljet_m, &b_ljet_m);
  // cTree->SetBranchAddress("ljet_sd12", &ljet_sd12, &b_ljet_sd12);
}

/*
  weight_mc ---> these are the MC weights. They are useful to match MC samples with different weights.
  weight_pileup--> weights to match the pile-up distribution between data and MC
  weight_jvt --> if you select small-R jets, you need to apply this weight to correct by differences on the jvt efficiency between data and MC.
  weight_bTagSF_70-> event weight that you need to apply if you apply b-tagging 70% requirement on small-R jets. (you will see also the variables with _77, _85)
  weight_trackjet_bTagSF_70-> the same than before but applying 70% W.P. on track jets.
  weight_leptonSF->lepton scale factors to correct the MC efficiency to the data.
*/

/*
  ljet variables are for large-R jet distributions, jet variables are for small-R jet distributions.
*/



#endif /*TREECONNECTOR_H*/
