--TEST--
Combination of foreach, finally and goto
--FILE--
<?php

function fn752531324()
{
    foreach ([new stdClass()] as $a) {
        try {
            foreach ([new stdClass()] as $a) {
                try {
                    foreach ([new stdClass()] as $a) {
                        goto out;
                    }
                } finally {
                    echo "finally1\n";
                }
                out:
            }
        } finally {
            echo "finally2\n";
        }
    }
}
fn752531324();
--EXPECT--
finally1
finally2
