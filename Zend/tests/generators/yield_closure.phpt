--TEST--
Generator shouldn't crash if last yielded value is a closure
--FILE--
<?php

function gen()
{
    (yield function () {
    });
}
function fn358672391()
{
    $gen = gen();
    $gen->next();
    echo "Done!";
}
fn358672391();
--EXPECT--
Done!
