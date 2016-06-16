--TEST--
Bug #70228 (memleak if return in finally block)
--FILE--
<?php

function test()
{
    try {
        throw new Exception(1);
    } finally {
        try {
            try {
            } finally {
                return 42;
            }
        } finally {
            throw new Exception(2);
        }
    }
}
function fn963533412()
{
    try {
        var_dump(test());
    } catch (Exception $e) {
        do {
            echo $e->getMessage() . "\n";
            $e = $e->getPrevious();
        } while ($e);
    }
}
fn963533412();
--EXPECT--
2
1
