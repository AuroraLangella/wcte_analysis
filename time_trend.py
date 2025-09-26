import uproot 
import matplotlib.pyplot as plt
import numpy as np

#events = uproot.open("/home/promano/wcte_analysis/runs_by_mPMT/mPMT1_run045.root")
events = uproot.open("/storage/wcte-recon/runs_by_mPMT/mPMT1_run105.root")
events2 = uproot.open("/storage/wcte-recon/runs_by_mPMT/sorted_prova.root")
data = events["data"]
data2 = events2["data"]
#print(events.classnames())
#print(data)
unixtime= data["UnixTime"].array(library='np')
unixtime2 = data2["UnixTime"].array(library='np')
x = range(len(unixtime))
#plt.scatter(x,unixtime)
plt.plot(unixtime, ls='', marker='o')
plt.plot(unixtime2, ls='', marker='o',label='sorted')
plt.legend()
plt.show()
