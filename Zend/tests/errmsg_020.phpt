--TEST--
errmsg: disabled function
--INI--
disable_functions=phpinfo
--FILE--
<?php

function fn969568591()
{
    phpinfo();
    echo "Done\n";
}
fn969568591();
--EXPECTF--	
Warning: phpinfo() has been disabled for security reasons in %s on line %d
Done
