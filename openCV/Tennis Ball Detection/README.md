# Tennis Ball Detection Task:  
### Videos:
- [ ] [Well-Lit](https://youtu.be/9syLASs08A0)
- [ ] [Dimly-Lit](https://youtu.be/QC732p7u2U4)
-----------------------------------------------------------------------------------------------------------------------------------------
### Screenshots:
- Well Lit 1: ![Well Lit 1](https://media.discordapp.net/attachments/1224426436153315391/1224426708141215884/image.png?ex=661d733b&is=660afe3b&hm=c851d13979766ba37995f72418349461b38d6e03f942c11e4f2c83e6dbe2ce9e&=&format=webp&quality=lossless&width=1751&height=669)
- Well Lit 2: ![Well Lit 2](https://media.discordapp.net/attachments/1224426436153315391/1224426614134276237/image.png?ex=661d7325&is=660afe25&hm=54f14e7b434b37ffebee5161533fa4a26fcce76b65feee1e8e9007c0aabd627e&=&format=webp&quality=lossless&width=1760&height=668)
- Dimly Lit 1:![Dimly Lit 1](https://media.discordapp.net/attachments/1224426436153315391/1224426889146273874/image.png?ex=661d7367&is=660afe67&hm=9d4f2f80899e6c5ec62df0b94dc977b09de1c4d4886c8db9064d07f0bcbb314d&=&format=webp&quality=lossless&width=1762&height=668)
- Dimly Lit 2: ![Dimly Lit 2](https://media.discordapp.net/attachments/1224426436153315391/1224427050446749828/image.png?ex=661d738d&is=660afe8d&hm=9ab2925b7a2879d6dc73db6098b74a27de510bf47c6eee2a55fd7a8959dc6a13&=&format=webp&quality=lossless&width=1758&height=683)
- Dimly Lit 3: ![enter image description here](https://media.discordapp.net/attachments/1224426436153315391/1224427099373310083/image.png?ex=661d7399&is=660afe99&hm=eadd52cbdcb5b214d8941f94a448b870dfec8b35756158dec0044142f55ce0b1&=&format=webp&quality=lossless&width=1782&height=677)
- ---------------------------------------------------------------------------------------------------------------------------------------



### Process:
- [ ] Denoising is done on the webcam feed using gaussian blur + median blur .
- [ ] The frames are converted to HSV. A mask is then created to help identify the required colour. Erosion and dilation help improve the mask.
- [ ] Hough circles help identify the right shape.  



