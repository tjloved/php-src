--TEST--
Try finally (with near goto)
--FILE--
<?php

function foo()
{
    $jmp = 1;
    try {
    } finally {
        previous:
        if ($jmp) {
            goto label;
            echo "dummy";
            label:
            echo "label\n";
            $jmp = 0;
            goto previous;
        }
        echo "okey";
    }
}
function fn2147026009()
{
    foo();
}
fn2147026009();
--EXPECTF--
label
okey
