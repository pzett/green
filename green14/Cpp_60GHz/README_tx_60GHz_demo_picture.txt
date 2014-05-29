%%%% README.txt %%%%

Short Doc how to use the code:

- Compile with: command: make

- run with: command: sudo ./tx_60GHz

- options: --demoMode false ; makes the pictures not to pop up

- Never do command: ./genRand except you want to generate a new pseudonoise (PS) sequence. When you do so, make sure to have the same PS at the receiver

- To change the picture, open image_transmission.m and modify 'Obama.png' with the name of your picture. The picture should be in grey scale, .png and will be converted to 128*128 by octave.



