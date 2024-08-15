from ROOT import TFile, TH1F, TCanvas, std, gPad
import os
import numpy as np
from array import array

cwd = os.getcwd()
c = TCanvas("c1", "deposited energy", 2000, 1000)
for root, dirs, files in os.walk(cwd):
     for dir in dirs:
        # print("folderul curent este "+ dir)
        os.chdir(dir)
        file = TFile("scoring.root", "READ")
        h_1 = TH1F('eDep','Energy Deposition',1000, 0., 2000.)
        h_1.GetXaxis().SetRangeUser(0,2.0)
        t1 = file.Get('betaBack_eDep')
        # energy = np.array(0)
        # t1.SetBranchAddress("betaBack_eDep_score", energy)
        leaf = t1.Draw("betaBack_eDep_score>>h_1", "", "")
        gPad.GetPrimitive("eDep")
        
        # Set the x-axis range
        # gPad.GetXaxis().SetRangeUser(0, 20)
        # for i in range(t1.GetEntries()):
        #     t1.GetEntry(i)
        #     h_1.Fill(energy)
        gPad.Update()
        # gPad.GetPrimitive("htemp")
        # 
        # h_1.Draw()
        c.Update()
        c.SaveAs("../{}.png".format(dir))
        os.chdir(cwd)        
# c.SetLogy()
c.Draw()
c.SaveAs("pic1.png")