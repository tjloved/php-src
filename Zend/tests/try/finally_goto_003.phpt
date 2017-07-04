--TEST--
jmp into a finally block 03
--FILE--
<?php

function foo()
{
    try {
    } finally {
        goto test;
        test:
    }
}
function fn995179926()
{
    echo "okey";
}
fn995179926();
--EXPECTF--
okey
