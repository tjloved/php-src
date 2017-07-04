--TEST--
foreach() (and other) variables aren't leaked on premature close
--FILE--
<?php

function gen(array $array)
{
    foreach ($array as $value) {
        (yield $value);
    }
}
function fn1044629295()
{
    $gen = gen(['Foo', 'Bar']);
    var_dump($gen->current());
    // generator is closed here, without running SWITCH_FREE
}
fn1044629295();
--EXPECT--
string(3) "Foo"
