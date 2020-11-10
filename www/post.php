<?php
if (isset($_POST["name"]))
{
	echo 'Bonjour ' . htmlspecialchars($_POST["name"]) . '!';
}
else
{
	http_response_code(404);
}
?>