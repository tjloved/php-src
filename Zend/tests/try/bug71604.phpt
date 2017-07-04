--TEST--
Bug #71604: Aborted Generators continue after nested finally
--FILE--
<?php

function gen()
{
    try {
        try {
            yield;
        } finally {
            print "INNER\n";
        }
    } catch (Exception $e) {
        print "EX\n";
    } finally {
        print "OUTER\n";
    }
    print "NOTREACHED\n";
}
function fn171761467()
{
    gen()->current();
}
fn171761467();
--EXPECT--
INNER
OUTER
