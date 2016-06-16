--TEST--
Bug #72213 (Finally leaks on nested exceptions)
--FILE--
<?php

function test()
{
    try {
        throw new Exception(1);
    } finally {
        try {
            try {
                throw new Exception(2);
            } finally {
            }
        } catch (Exception $e) {
        }
    }
}
function fn253836216()
{
    try {
        test();
    } catch (Exception $e) {
        echo "caught {$e->getMessage()}\n";
    }
}
fn253836216();
--EXPECT--
caught 1
