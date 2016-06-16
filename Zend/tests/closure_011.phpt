--TEST--
Closure 011: Lexical copies not static in closure
--FILE--
<?php

function fn1488642561()
{
    $i = 1;
    $lambda = function () use($i) {
        return ++$i;
    };
    $lambda();
    echo $lambda() . "\n";
    //early prototypes gave 3 here because $i was static in $lambda

}
fn1488642561();
--EXPECT--
2
