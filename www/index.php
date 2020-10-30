<html>
<!-- Text between angle brackets is an HTML tag and is not displayed.
Most tags, such as the HTML and /HTML tags that surround the contents of
a page, come in pairs; some tags, like HR, for a horizontal rule, stand 
alone. Comments, such as the text you're reading, are not displayed when
the Web page is shown. The information between the HEAD and /HEAD tags is 
not displayed. The information between the BODY and /BODY tags is displayed.-->
<head>
<title>Example de page</title>
</head>
<!-- The information between the BODY and /BODY tags is displayed.-->
<body>
<h1>Cette page vous a ete fourni par ce micro-weberver :)</h1>
<ul>
<li>lul je fais du front-end</li>
<li>c nul le html <i>italicize</i> key words</li>
</ul>
<?php
if (isset($_GET["name"]))
{
	echo 'Bonjour ' . htmlspecialchars($_GET["name"]) . '!';
}
else
{
	echo 'Bonjour inconnu !';	
}
?>
<p>Flemme de modifier le reste </p>
<p><img src="http://www.mygifs.com/CoverImage.gif" alt="A Great HTML Resource"></p>
<p>Add a link to your favorite <a href="https://www.dummies.com/">Web site</a>.
Break up your page with a horizontal rule or two. </p>
<hr>
<p>Finally, link to <a href="page2.html">another page</a> in your own Web site.</p>
<!-- And add a copyright notice.-->
<p>&#169; sel-melc, 2020</p>
</body>
</html>