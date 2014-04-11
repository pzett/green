function data=generate_data (nData)
%Generates binary data quantized from a Gaussian zero mean and unit
%variance random variable with a 1-bit quantizer.

b=randn(1,nData);
b=b-mean(b);
b=b/std(b);
data = (b > 0);

