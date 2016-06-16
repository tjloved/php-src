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
function fn523688141()
{
    echo "okey";
}
fn523688141();
--EXPECTF--
okey
