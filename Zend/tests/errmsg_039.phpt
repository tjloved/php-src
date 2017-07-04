--TEST--
errmsg: cannot redeclare property
--FILE--
<?php

class test
{
    var $var;
    var $var;
}
function fn1840346026()
{
    echo "Done\n";
}
fn1840346026();
--EXPECTF--	
Fatal error: Cannot redeclare test::$var in %s on line %d
