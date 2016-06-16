--TEST--
Bug #36303 (foreach on error_zval produces segfault)
--FILE--
<?php

function fn1135824273()
{
    $x = "test";
    foreach ($x->a->b as &$v) {
    }
    echo "ok\n";
}
fn1135824273();
--EXPECTF--
Warning: Attempt to modify property of non-object in %sbug36303.php on line %d

Warning: Invalid argument supplied for foreach() in %sbug36303.php on line %d
ok
