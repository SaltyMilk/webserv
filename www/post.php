<?php
if (isset($_POST["name"]))
{
	echo 'Bonjour ' . htmlspecialchars($_POST["name"]) . '!';
}
else
{
	echo 'Bonjour inconnu !';	
}
?>