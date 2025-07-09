package inet.mobility.single;

import inet.mobility.base.MovingMobilityBase;

//
// Mobility model that walks through street junctions loaded from a CSV.
//
// 
simple KogakuinMobility extends MovingMobilityBase
{
    parameters:
        string traceFile;        // path to CSV file of junctions and neighbors
        double speed @unit(mps) = default(100mps); // movement speed in meters per second
        double referenceLatitude  = default(NaN);
        double referenceLongitude = default(NaN);
		double initialX @unit(m) = default(uniform(constraintAreaMinX, constraintAreaMaxX));
        double initialY @unit(m) = default(uniform(constraintAreaMinY, constraintAreaMaxY));
        double initialZ @unit(m) = default(uniform(constraintAreaMinZ, constraintAreaMaxZ));
        @display("p=150,100");
        @class(KogakuinMobility);
}
