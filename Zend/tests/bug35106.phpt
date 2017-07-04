--TEST--
Bug #35106 (nested foreach fails when array variable has a reference)
--FILE--
<?php

function fn1653334006()
{
    $a = array("1", "2");
    $b =& $a;
    foreach ($a as $i) {
        echo $i;
        foreach ($a as $p) {
        }
    }
    echo "\n";
}
fn1653334006();
--EXPECT--
12
