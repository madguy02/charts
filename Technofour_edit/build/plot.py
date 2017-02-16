import pandas as pd
import matplotlib.pyplot as plt
df  = pd.read_csv("data.csv")
df.plot()  # plots all columns against index
df.plot(kind='scatter',x='x',y='y') # scatter plot
df.plot(kind='density')