--TEST--
try { return } finally { return } in generator
--FILE--
<?php

function gen()
{
    try {
        try {
            echo "before return\n";
            return;
            echo "after return\n";
        } finally {
            echo "before return in inner finally\n";
            return;
            echo "after return in inner finally\n";
        }
    } finally {
        echo "outer finally run\n";
    }
    echo "code after finally\n";
    yield;
    // force generator
}
function fn443861514()
{
    $gen = gen();
    $gen->rewind();
    // force run
}
fn443861514();
--EXPECTF--
before return
before return in inner finally
outer finally run
