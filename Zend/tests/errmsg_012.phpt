--TEST--
errmsg: __autoload() must take exactly 1 argument
--FILE--
<?php

function __autoload($a, $b)
{
}
function fn259635908()
{
    echo "Done\n";
}
fn259635908();
--EXPECTF--	
Fatal error: __autoload() must take exactly 1 argument in %s on line %d
