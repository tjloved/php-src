--TEST--
yield can be used in finally (apart from forced closes)
--FILE--
<?php

function gen()
{
    try {
        echo "before return\n";
        return;
        echo "after return\n";
    } finally {
        echo "before yield\n";
        (yield "yielded value");
        echo "after yield\n";
    }
    echo "after finally\n";
}
function fn575759135()
{
    $gen = gen();
    var_dump($gen->current());
    $gen->next();
}
fn575759135();
--EXPECTF--
before return
before yield
string(%d) "yielded value"
after yield
