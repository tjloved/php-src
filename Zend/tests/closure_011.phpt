--TEST--
Closure 011: Lexical copies not static in closure
--FILE--
<?php

function fn965544886()
{
    $i = 1;
    $lambda = function () use($i) {
        return ++$i;
    };
    $lambda();
    echo $lambda() . "\n";
    //early prototypes gave 3 here because $i was static in $lambda
}
fn965544886();
--EXPECT--
2
