import os
import ROOT

def find_root_files(directory):
    """Recursively find all ROOT files in the given directory."""
    root_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".root"):
                root_files.append(os.path.join(root, file))
    return root_files

def plot_ttrees_on_canvas(directory, tree_name, branch_name, nbins, xmin, xmax):
    """Scroll through directories, plot branches from TTrees in ROOT files on different pads in the same canvas."""
    # Create a list to store histograms
    histograms = []

    # Find all ROOT files in the directory
    root_files = find_root_files(directory)

    # Check if any ROOT files were found
    if len(root_files) == 0:
        print("No ROOT files found in the directory.")
        return

    # Create a canvas and divide it into sub-pads
    canvas = ROOT.TCanvas("canvas", "Canvas", 1200, 800)
    npads = len(root_files)
    canvas.Divide(1, npads)  # 1 column, npads rows

    # Loop over each ROOT file
    for i, root_file in enumerate(root_files):
        print(f"Processing file: {root_file}")

        # Open the ROOT file
        file = ROOT.TFile.Open(root_file)
        if not file or file.IsZombie():
            print(f"Could not open file {root_file}")
            continue

        # Get the TTree
        tree = file.Get(tree_name)
        if not tree:
            print(f"TTree {tree_name} not found in {root_file}")
            file.Close()
            continue

        # Check if the TTree has entries
        nentries = tree.GetEntries()
        print(f"Number of entries in {tree_name}: {nentries}")
        if nentries == 0:
            print(f"TTree {tree_name} in file {root_file} has no entries.")
            file.Close()
            continue

        # Create a histogram for this file
        histogram = ROOT.TH1F(f"histogram_{i}", f"Histogram {i+1}", nbins, xmin, xmax)

        # Fill the histogram with the branch data
        for j in range(nentries):
            tree.GetEntry(j)
            value = getattr(tree, branch_name, None)
            if value is not None:
                histogram.Fill(value)
                # print(f"Filled histogram with value: {value}")
            else:
                print(f"Value for {branch_name} is None in entry {j}")

        # Add the histogram to the list
        histograms.append(histogram)

        # Draw the histogram on the corresponding pad
        canvas.cd(i + 1)
        histogram.Draw()

        # Close the ROOT file
        file.Close()

    # Update the canvas and save it as an image
    canvas.Update()
    canvas.SaveAs("multi_pad_histograms.png")

if __name__ == "__main__":
    directory = os.getcwd()
    tree_name = "betaFront_eDep"      # Replace with the actual TTree name
    branch_name = "betaFront_eDep_score"  # Replace with the actual branch name
    nbins = 100                       # Number of bins in the histogram
    xmin = 0                          # Minimum x-axis value
    xmax = 10                        # Maximum x-axis value

    plot_ttrees_on_canvas(directory, tree_name, branch_name, nbins, xmin, xmax)
