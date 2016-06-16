--TEST--
output buffering - ob_get_clean
--FILE--
<?php

function fn478466652()
{
    ob_start();
    echo "foo\n";
    var_dump(ob_get_clean());
}
fn478466652();
--EXPECT--
string(4) "foo
"
