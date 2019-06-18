#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
//#include "TPaveStats.h"
#include "TPaveText.h"
#include "TString.h"
#include "TGraph.h"
#include "TMultiGraph.h"
//#include "TLatex.h"


int main(int argc, char* argv[])
{
  if (argc == 1) { std::cout << "Arguments needed" << std::endl; exit(1); }

  std::vector<TString> files;
  TString arg;

  for (int i = 1; i < argc; ++i)
    {
      arg = argv[i];

      if (arg.Contains(".root")) files.push_back(arg);
      else {std::cout << "Argument " << arg << "is not a root file!" << std::endl; exit(1);}
    }

  TCanvas *canvas = new TCanvas("canvas", "SD", 900, 700);
  TFile *f = 0;
  TGraph *g = 0;
  TLegend *leg = new TLegend(.65,.65,.9,.9);
  TString runNum, parameters, jetTypeUntrim, jetTypeTrim, minPt, maxPt;
  Ssiz_t begin;                    //begin is the index at the beginning of the parameters

  for (int i = 0; i < files.size(); ++i)
    {
      f = TFile::Open(files[i], "READ");
      if (!f) {std::cout << "File was not opened!" << std::endl; exit(1);}
      
      begin = files[i].First('_') + 1;
      parameters = files[i](begin, files[i].Length() - begin); parameters = parameters(0, parameters.Length() - 5);

      runNum = files[i](3, begin - 4);
      jetTypeUntrim = parameters(0, parameters.Index("Pt", 2, 0, TString::kExact) - 1);
      jetTypeTrim = (jetTypeUntrim.Copy()).ReplaceAll("_", " ");
      minPt = parameters(parameters.Index("Pt",2,0,TString::kExact) + 2, 4); if (minPt[0] == '0') { minPt = minPt.Remove(TString::kLeading, '0'); }
      maxPt = parameters(parameters.Index("Pt",2,0,TString::kExact) + 7, 4); 


      std::cout << "FILE " << i+1 << "'S CONTENTS:" << std::endl;
      f->ls();
      std::cout << std::endl;
      
      f->GetObject("roc", g);
      if (g && i == 0) 
	{
	  g->SetLineColor(2+i); g->Draw();
	  leg->AddEntry(g, "#"+runNum+" "+jetTypeTrim+" Pt: "+minPt+" - "+maxPt, "l");
	}
      else if (g && i > 0)
	{
	  g->SetLineColor(2+i); g->Draw("SAME");
	  leg->AddEntry(g, "#"+runNum+" "+jetTypeTrim+" Pt: "+minPt+" - "+maxPt, "l");
	}
      else { std::cout << "Graph was not retrieved from the file!" << std::endl; }

      leg->Draw();
      canvas->Update();
      f->Close();
    }

  canvas->SetLogy();
  canvas->Print("ROC_Overlay.png");
}
