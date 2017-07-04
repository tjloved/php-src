--TEST--
Bug #35411 (Regression with \{$ handling)
--FILE--
<?php

function fn1680813606()
{
    $abc = "bar";
    echo "foo\\{{$abc}}baz\n";
    echo "foo\\{ {$abc}}baz\n";
    echo <<<TEST
foo{$abc}baz
foo\\{{$abc}}baz
foo\\{ {$abc}}baz
TEST;
}
fn1680813606();
--EXPECT--
foo\{bar}baz
foo\{ bar}baz
foobarbaz
foo\{bar}baz
foo\{ bar}baz
