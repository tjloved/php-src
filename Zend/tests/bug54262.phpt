--TEST--
Bug #54262 (Crash when assigning value to a dimension in a non-array)
--FILE--
<?php

function fn1648026716()
{
    $a = '0';
    var_dump(isset($a['b']));
    $simpleString = preg_match('//', '', $a->a);
    $simpleString["wrong"] = "f";
    echo "ok\n";
}
fn1648026716();
--EXPECTF--
bool(false)

Warning: Attempt to modify property of non-object in %sbug54262.php on line %d

Warning: Cannot use a scalar value as an array in %sbug54262.php on line %d
ok
