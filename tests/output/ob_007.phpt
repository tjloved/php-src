--TEST--
output buffering - ob_get_clean
--FILE--
<?php

function fn9424286()
{
    ob_start();
    echo "foo\n";
    var_dump(ob_get_clean());
}
fn9424286();
--EXPECT--
string(4) "foo
"
