<table style="width:1000px; border: 0px solid black;">
<tr style="border: 0px solid black;">
<td style="border: 0px solid black;">
<center>
<img src="https://github.com/Techbot/techtools-for-vcvrack/blob/main/img/Techbot.png" alt="Techbot">
<img src="https://github.com/Techbot/techtools-for-vcvrack/blob/main/img/vcvrack.png" alt="VCVrack Plugin">
<h1 style="border-bottom: 0px;font-size:50px;">Techtools For VCVRack</h1>
<h2 style="border-bottom: 0px;">Plugin modules for VCV Rack v2 by @Techbot</h2>
</center>
</td>
</tr>
</table>

Here's the deal. Libusb on windows seems to add several tenths of a milisecond over Linux to the tuning routine, making CV controlled tuning on windows nigh on impossible.

After a year on and off of making no progress until I doscovered this and switched back to Ubuntu, I don't have the energy to go digging into Windows impementation of Libusb. I'll continue developibng the device for Linux only and will revist in 6 months time, when I know multithreading better than I do now.


<h3 style="border-bottom: 0px;">Part I Glitch Rompler</h3>
<p>
Step 1 Extract Step Seq and sampler from Original Authors code, add dependencies, remove extra functions, hardcode definitions: DONE </br>
Step 2 Make Duphonic by duplicating play code in process function: DONE </br>
Step 3 Make Polyphonic replacing original & duplicated code with i loop over the 8 channels: DONE</br>
Step 4 Combine seq and sampler into one module: DONE </br>
Step 5 Remove intermediate inputs and outputs: DONE </br>
Step 6 Remove sample load option replace with rompler cycle option: DONE </br>
</p>

<h3 style="border-bottom: 0px;">Part II Radio Scanner</h3>
<p>
Step 1 Upgrade code from pre version 1 aka .06 to version 2.0</br>
Step 2 Multithreading</br>
Step 3 CV Scanning</br>
Step 4 Mux Demux for multiple channels?</br>
</p>

<h3 style="border-bottom: 0px;">Part III Combine Both</h3>
<p>
Step 1 Create Single plugin.dll</br>
Step 2 Create additional in and out CVs</br>
</p>

<hr style="width:1000px; border: 1px solid black;"/>
<h3>Licenses</h3>
<p>
Some source code in this repository is copyright © 2021 emc23dotcom and Techbot and licensed under GNU GPLv3
</p>
<p>
Some source code in this repository is copyright © 2021 Adam Verspaget/Count Modula and licensed under GNU GPLv3
</p>
<p>
Some source code in this repository is copyright (c) 2019, Clement Foulc and licensed underBSD 3-Clause License
</p>
<p>
All graphics including the Techbot logo, panels and components are copyright © 2021 emc23/techbot https://www.emc23.com
</p>
<p>
16 Step Rompler Glitch sounds taken from Left's Sound Design and contributions from the EMC meetup group
</p>

<p>
<img src="https://github.com/EMC23/techtools/blob/main/img/Techtools.png" alt="Techtools VCVrack Plugin">
</p>

<h3>Modules - Release 1.0.0</h3>
<p>
<a href="MANUAL.md">User Guides</a>
</p>
<ul>
<li> 16 Step 4 Voice 8 Channel Glitch Rompler</li>
</ul>
