from matplotlib import pyplot as plt
import pandas as pd
import sys

def main(cmd_args):
    try:
        _, *filenames = cmd_args
    except ValueError as e:
        raise ValueError("No '.csv' data files given for ploting!") from e
    plt.figure("Fuzzy sets")
    plt.title("Membership functions of fuzzy sets")
    plt.xlabel("Input variable [...]")
    plt.ylabel("Membership function [-]")
    for filename in filenames:
        try:
            data = pd.read_csv(filename, sep=";")
            plt.plot(
                data.iloc[:, 0], data.iloc[:, 1],
                label=filename.replace(".csv", "")
            )
        except FileNotFoundError as e:
            print(f"File '{filename}' not found! Skipping...")
    
    plt.legend()
    plt.show()

if __name__ == "__main__":
    main(sys.argv)