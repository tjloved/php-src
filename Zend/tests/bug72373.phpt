--TEST--
Bug #72373: TypeError after Generator function w/declared return type finishes
--FILE--
<?php

function foo() : Generator
{
    (yield 1);
    (yield 2);
    (yield 3);
}
function fn1770746531()
{
    foreach (foo() as $bar) {
        echo $bar . "\n";
    }
}
fn1770746531();
--EXPECT--
1
2
3
