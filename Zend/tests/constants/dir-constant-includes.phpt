--TEST--
__DIR__ constant test with includes
--FILE--
<?php

function fn1691848322()
{
    echo __DIR__ . "\n";
    echo dirname(__FILE__) . "\n";
    include 'fixtures/folder1/fixture.inc';
    include 'fixtures/folder2/fixture.inc';
    include 'fixtures/folder3/fixture.inc';
    include 'fixtures/folder4/fixture.inc';
}
fn1691848322();
--EXPECTF--
%stests%sconstants
%stests%sconstants
%stests%sconstants%sfixtures%sfolder1
%stests%sconstants%sfixtures%sfolder1
%stests%sconstants%sfixtures%sfolder2
%stests%sconstants%sfixtures%sfolder2
%stests%sconstants%sfixtures%sfolder3
%stests%sconstants%sfixtures%sfolder3
%stests%sconstants%sfixtures%sfolder4
%stests%sconstants%sfixtures%sfolder4
