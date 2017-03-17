Part 1 

Here we embed the watermark in an image in the frequency domain. We inject the watermark into some special frequencies of the image. Here is one way, by modifying the real portion of the fourier transform in a circular pattern of radius r (where r is another parameter).For instance,we could choose l evenly-spaced bins along the circle and modify each of those with one
bit of the sequence. To inject bit vi into bin R(x; y), set its new value to:

R'(x; y) = R(x; y) + a|R(x; y)|vi;

To code also test if watermark is present in the image or not.

In next section, we remove noise from noise1.png.


Part 2

We use convolutions and edge detection to try to find objects in images.We try to locate cars in satellite images.We used a "sliding window" approach, where you crop out a few car images manually to use as car "templates," and then used cross correlation to find local image regions that are similar to one or more of the templates.
