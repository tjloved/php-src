--TEST--
Bug #35106 (nested foreach fails when array variable has a reference)
--FILE--
<?php

function fn409680288()
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
fn409680288();
--EXPECT--
12
