--TEST--
Closure 003: Lambda with lexical variables (local scope)
--FILE--
<?php

function run()
{
    $x = 4;
    $lambda1 = function () use($x) {
        echo "{$x}\n";
    };
    $lambda2 = function () use(&$x) {
        echo "{$x}\n";
    };
    $lambda1();
    $lambda2();
    $x++;
    $lambda1();
    $lambda2();
}
function fn1973318959()
{
    run();
    echo "Done\n";
}
fn1973318959();
--EXPECT--
4
4
4
5
Done
