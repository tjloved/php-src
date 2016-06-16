--TEST--
errmsg: __autoload() must take exactly 1 argument
--FILE--
<?php

function __autoload($a, $b)
{
}
function fn796361244()
{
    echo "Done\n";
}
fn796361244();
--EXPECTF--	
Fatal error: __autoload() must take exactly 1 argument in %s on line %d
