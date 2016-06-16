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
function fn462568578()
{
    try {
        foo();
    } catch (Exception $ex) {
        echo "ok\n";
    }
}
fn462568578();
--EXPECT--
ok
