--TEST--
Bug #44184 (Double free of loop-variable on exception)
--FILE--
<?php

function foo()
{
    $x = array(1, 2, 3);
    foreach ($x as $a) {
        while (1) {
            throw new Exception();
        }
        return;
    }
}
function fn1491660679()
{
    try {
        foo();
    } catch (Exception $ex) {
        echo "ok\n";
    }
}
fn1491660679();
--EXPECT--
ok
