--TEST--
Generator shouldn't crash if last yielded value is a closure
--FILE--
<?php

function gen()
{
    (yield function () {
    });
}
function fn2054586699()
{
    $gen = gen();
    $gen->next();
    echo "Done!";
}
fn2054586699();
--EXPECT--
Done!
