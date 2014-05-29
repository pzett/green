% Script to transmit with the USRP

Nsamples=length(tx_data)
gain=0;
X=4400*tx_data/rms(tx_data);
tx_60GHz( Nsamples, X, gain)
