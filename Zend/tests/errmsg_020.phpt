--TEST--
errmsg: disabled function
--INI--
disable_functions=phpinfo
--FILE--
<?php

function fn1814703011()
{
    phpinfo();
    echo "Done\n";
}
fn1814703011();
--EXPECTF--	
Warning: phpinfo() has been disabled for security reasons in %s on line %d
Done
