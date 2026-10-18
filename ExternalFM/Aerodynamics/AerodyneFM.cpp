#include "AerodyneFM.h"
#include "AeroData.h"
#include "../ED_FM_Utility.h"
#include <cmath>
#include <vector>

void Hawk::Atmosphere::update(double temperature, double density, double vt, double* coeff)
{
	double temp, rho, mach, qbar;

	temp = temperature * 1.8;
	rho = density * 0.00194032033;
	mach = (vt) / sqrt(1.4 * 1716.3 * temp);
	qbar = .5 * rho * pow(vt, 2);

	coeff[0] = qbar;
	coeff[1] = mach;
}

void AerodyneFM::SetAerodynamicAngles(double aoa, double aos)
{
	alpha = aoa;
	beta = aos;
}

void AerodyneFM::SetAtmosphere(double altitude, double density, double temperature, double pressure, double vSound)
{
	quota_slm_m = altitude;
	densita = density;
	pressione = pressure;
}

void AerodyneFM::SetSurfaceHeight(double surfaceHeight)
{
	altezzaSuolo = surfaceHeight;
}

void AerodyneFM::SetCenterOfGravity(const Vec3& cg)
{
	center_of_gravity = cg;
}

void AerodyneFM::SetSpeeds(double v, double mach)
{
	V_scalar_MS = v;
	Mach = mach;
}

double AerodyneFM::getMachSpeed()
{
	// printf("Mach = %f \n", Mach);
	return Mach;
}

void AerodyneFM::SetFlightControls(double elevator, double aileron, double rudder)
{
	def_elev = elevator;
	def_aileron = aileron;
	def_rudder = rudder;
}

void AerodyneFM::SetFlapPerc(double flapPerc)
{
	flap_pos = flapPerc;
}

void AerodyneFM::SetSpeedBrakePerc(double sbPerc)
{
	sb_pos = sbPerc;
}
void AerodyneFM::SetGearPerc(double gearPerc)
{
	gear_pos = gearPerc;
}

double AerodyneFM::GetCockpitOverPressure()
{
	return overpressure;
}

Vec3 AerodyneFM::GetForces()
{
	return Vec3(resultX, resultY, resultZ);
}

Vec3 AerodyneFM::GetMoments()
{
	return Vec3(L, M, N);
}

void AerodyneFM::SetBodyAxis(double u, double v, double w, double udot, double wdot, double p, double q, double r)
{
	u_ = u;
	v_ = v;
	w_ = w;
	udot_ = udot;
	wdot_ = wdot;
	p_ = p;
	q_ = q;
	r_ = r;
}

void AerodyneFM::Update(double dt)
{	
	/*------------------------------------*/
	/*----------Variabili locali----------*/
	/*------------------------------------*/
	//TODO sistemare la riga successiva
	double	pi = 4.0*atan(1.0);		// Pi (3.14159....)
	double  compr = 0.0;			// fattore di correzione CL e CD per compressibilit�
	double	alphaDot = 0.0;			// in [rad/s]	-	velocit� di variazione AoA
	double	alphaDot_seg = 0.0;		// velocit� di variazione AoA normalizzata
	double	phiGroundEffect = 0.0;

	double  CL = 0.0;
	double  CD = 0.0, CDEff;
	double	Cm = 0.0;
	double	CLtot = 0.0;
	double	CDtot = 0.0;
	double	CYtot = 0.0;
	double	Cltot = 0.0;
	double	Cntot = 0.0;
	double	Cmtot = 0.0;
	double	CnYawEff;
	double	ClYawEff;
	double	CmdeEff;
	double	CmqEff, CmqEff2;
	double	CmalphaDotEff, CmalphaDotEff2;
	double	CnDrEff;
	double	ClBetaEff;
	double	ClDaEff;
	double	CmalphaEff, ClRollEff, CnBetaEff, CnBetaEff2;

	double	rr = 0.0;
	double	pp = 0.0;
	double	qq = 0.0;


	// TODO Verificare se c'e gia una funzione pronta. Altrimenti sostituire "pi" con M_PI da math. Anzi, creare la funzione in Utility.h
	double alphaDeg = alpha * 180.0 / pi;

	double row = -1;

	// Interpolazione coefficienti aerodinamici a seconda dell'AoA
	for (int i = 0; i < aerodynamics.size() - 1; i++)
	{
		if (aerodynamics[i].mAoa <= alphaDeg)
		{
			row = i;
			// TODO Solving the performance issue: Store the i value in another variable (e.g. "j") and perform all the following calculation AFTER the for

		}
	}
	double aoaRadCurrentRow = aerodynamics[row].mAoa * pi / 180.0;
	double aoaRadNextRow = aerodynamics[row + 1].mAoa * pi / 180.0;

	double tempVal = 0.87;

	CL = linearInterpolation(aerodynamics[row].mCl, aerodynamics[row + 1].mCl, alpha, aoaRadCurrentRow, aoaRadNextRow);
	CD = linearInterpolation(aerodynamics[row].mCd, aerodynamics[row + 1].mCd, alpha, aoaRadCurrentRow, aoaRadNextRow);
	
	row = -1;

	//Cmde = tempVal * linearInterpolation(aerodynamics[i].mCmde, aerodynamics[i + 1].mCmde, alpha, aoaRadCurrentRow, aoaRadNextRow);
	//CmalphaEff = Cmalpha *linearInterpolation(aerodynamics[i].mCmascal, aerodynamics[i + 1].mCmascal, alpha, aoaRadCurrentRow, aoaRadNextRow);



	// calcolo contributo Effetto Suolo - Teoria di McCormick
	// TODO Wrap in a method (e.g. GroundEffect) when moved into FlightModel class
	// TODO Performance issue: calculate the pow just once
	phiGroundEffect = pow((16.0 * (quota_slm_m - altezzaSuolo) / W_span), 2) / (1.0 + pow((16.0 * (quota_slm_m - altezzaSuolo) / W_span), 2));

	// TODO Use alphaDeg instead
	if ((quota_slm_m - altezzaSuolo) < W_span)
	{
		CD = CD0 + phiGroundEffect * (pow((CL + CLde_flap * flap_pos), 2) / (pi * e * AR));
	}
	
	// aggiungo fattore di compressibilit� ai CD
	// TODO Move the common part outside (CL = ...; and CD = ...) . Also, in C exists the /= operator (eg: CL /= compr; )
	if (Mach < 1.0)
	{
		compr = sqrt(1 - Mach * Mach);
	}
	else if (Mach > 1.0)
	{
		compr = sqrt(Mach * Mach - 1);
	}
	else
	{
		// la riga commentata sotto creava problemi
		//compr = (sqrt(1 - 0.99 * 0.99) + sqrt(1.01 * 1.01 - 1)) / 2.0 - 0.02;
		compr = sqrt(1 - 0.99 * 0.99);
	}
	CD = CD / compr;

	// introduco curva aerofreno per dare un po' di inerzia alla frenata
	for (int i = 0; i < scalarSpeedbrake.size() - 1; i++)
	{
		if (scalarSpeedbrake[i].mSbPos <= sb_pos)
		{
			row = i;
		}
	}
	sb_posEff = linearInterpolation(scalarSpeedbrake[row].mSbScalar, scalarSpeedbrake[row + 1].mSbScalar, sb_pos, scalarSpeedbrake[row].mSbPos, scalarSpeedbrake[row + 1].mSbPos);

	row = -1;

	// Interpolazione scalari dei vari coefficienti
	for (int i = 0; i < scalarCoef.size() - 1; i++)
	{
		// TODO Solving the performance issue: Store the i value in another variable (e.g. "j") and perform all the following calculation AFTER the for
		if ((scalarCoef[i].mAoa * pi / 180.0) <= alpha)
		{
			row = i;
		}
	}
	aoaRadCurrentRow = scalarCoef[row].mAoa * pi / 180.0;
	aoaRadNextRow = scalarCoef[row + 1].mAoa * pi / 180.0;

	ClRollEff = ClRoll * linearInterpolation(scalarCoef[row].mClp, scalarCoef[row + 1].mClp, alpha, aoaRadCurrentRow, aoaRadNextRow);
	CnBetaEff = CnBeta * linearInterpolation(scalarCoef[row].mCnb, scalarCoef[row + 1].mCnb, alpha, aoaRadCurrentRow, aoaRadNextRow);
	CmdeEff = Cmde * linearInterpolation(scalarCoef[row].mCmde, scalarCoef[row + 1].mCmde, alpha, aoaRadCurrentRow, aoaRadNextRow);
	CnYawEff = CnYaw * linearInterpolation(scalarCoef[row].mCnr, scalarCoef[row + 1].mCnr, alpha, aoaRadCurrentRow, aoaRadNextRow);
	CmqEff = Cmq * linearInterpolation(scalarCoef[row].mCmq, scalarCoef[row + 1].mCmq, alpha, aoaRadCurrentRow, aoaRadNextRow);
	CmalphaDotEff = CmalphaDot * linearInterpolation(scalarCoef[row].mCmadot, scalarCoef[row + 1].mCmadot, alpha, aoaRadCurrentRow, aoaRadNextRow);
	CnDrEff = CnDr * linearInterpolation(scalarCoef[row].mCndr, scalarCoef[row + 1].mCndr, alpha, aoaRadCurrentRow, aoaRadNextRow);
	ClBetaEff = ClBeta * linearInterpolation(scalarCoef[row].mClbeta, scalarCoef[row + 1].mClbeta, alpha, aoaRadCurrentRow, aoaRadNextRow);
	ClYawEff = ClYaw;// valutare se aggiungere la parte interpolante
	ClDaEff = ClDa * linearInterpolation(scalarCoef[row].mClda, scalarCoef[row + 1].mClda, alpha, aoaRadCurrentRow, aoaRadNextRow);

	row = -1;

	for (int i = 0; i < scalarDuetoMach.size() - 1; i++)
	{
		if (scalarDuetoMach[i].mMach <= Mach)
		{
			row = i;
		}
	}

	CmalphaDotEff2 = CmalphaDotEff * linearInterpolation(scalarDuetoMach[row].mCmadot, scalarDuetoMach[row + 1].mCmadot, Mach, scalarDuetoMach[row].mMach, scalarDuetoMach[row + 1].mMach);
	CmqEff2 = CmqEff * linearInterpolation(scalarDuetoMach[row].mCmq, scalarDuetoMach[row + 1].mCmq, Mach, scalarDuetoMach[row].mMach, scalarDuetoMach[row + 1].mMach);
	CmalphaEff = Cmalpha * linearInterpolation(scalarDuetoMach[row].mCma, scalarDuetoMach[row + 1].mCma, Mach, scalarDuetoMach[row].mMach, scalarDuetoMach[row + 1].mMach);
	CnBetaEff2 = CnBetaEff * linearInterpolation(scalarDuetoMach[row].mCnb, scalarDuetoMach[row + 1].mCnb, Mach, scalarDuetoMach[row].mMach, scalarDuetoMach[row + 1].mMach);

	row = -1;

	//TODO questo di seguito era un'inizializzazione che serviva per i danni
	double CnDaEff = CnDa, ClDrEff = ClDr;


	// calcolo effetto aeroelastico della superficie
	double CmdeEff2, ClDaEff2, CnDaEff2, CnDrEff2, ClDrEff2;
	double qinf = 0.5 * densita * V_scalar_MS * V_scalar_MS;
	//TODO inserire size of in caso
	for (int i = 0; i < ScalarDynPress.size() -1; i++)
	{
		if (ScalarDynPress[i].mQinf <= qinf)
		{
			row = i;
		}
	}
	CmdeEff2 = CmdeEff * linearInterpolation(ScalarDynPress[row].mScalarEl, ScalarDynPress[row + 1].mScalarEl, qinf, ScalarDynPress[row].mQinf, ScalarDynPress[row + 1].mQinf);	// *(ElevScalarDynPress[i].mScalar + ((ElevScalarDynPress[i + 1].mScalar - ElevScalarDynPress[i].mScalar) * ((qinf - ElevScalarDynPress[i].mQinf) / (ElevScalarDynPress[i + 1].mQinf - ElevScalarDynPress[i].mQinf))));
	ClDaEff2 = ClDaEff * linearInterpolation(ScalarDynPress[row].mScalarAil, ScalarDynPress[row + 1].mScalarAil, qinf, ScalarDynPress[row].mQinf, ScalarDynPress[row + 1].mQinf);
	CnDaEff2 = CnDaEff * linearInterpolation(ScalarDynPress[row].mScalarAil, ScalarDynPress[row + 1].mScalarAil, qinf, ScalarDynPress[row].mQinf, ScalarDynPress[row + 1].mQinf);
	CnDrEff2 = CnDrEff * linearInterpolation(ScalarDynPress[row].mScalarRud, ScalarDynPress[row + 1].mScalarRud, qinf, ScalarDynPress[row].mQinf, ScalarDynPress[row + 1].mQinf);
	ClDrEff2 = ClDrEff * linearInterpolation(ScalarDynPress[row].mScalarRud, ScalarDynPress[row + 1].mScalarRud, qinf, ScalarDynPress[row].mQinf, ScalarDynPress[row + 1].mQinf);
	
	row = -1;


	// Calcolo termini aerodinamici
	// Calcoli preliminari
	double V_2 = V_scalar_MS * 2;
	double VHalf = 0.5 * V_scalar_MS;
	double qTotS = 0.5 * densita * V_scalar_MS * V_scalar_MS * S;
	double qTotSWingSpan = qTotS * W_span;
	//TODO sistemare V_scalar_MS per evitare valore 0
	alphaDot = 0.0;		//era -> alphaDot = (u_*wdot_ - udot_ * w_) / (V_scalar_MS*V_scalar_MS*cos(beta)*cos(beta));
	pp = 0;
	qq = 0;
	rr = 0;
	alphaDot_seg = 0;

	if (V_scalar_MS != 0)
	{
		pp = p_ * (W_span / (2 * V_scalar_MS));
		qq = q_ * (MAC / (2 * V_scalar_MS));
		rr = r_ * (W_span / (2 * V_scalar_MS));
		alphaDot = (u_*wdot_ - udot_ * w_) / (V_scalar_MS*V_scalar_MS*cos(beta)*cos(beta));
		alphaDot_seg = alphaDot * (MAC / (2 * V_scalar_MS));
	}

	// Calcolo coefficienti di forza	
	CLtot = CL + CLde * def_elev + CLq * qq + CLalphaDot * alphaDot_seg + CLde_flap * flap_pos;

	CDtot = CD + CDDf * flap_pos + CDGear * gear_pos + CDsb * sb_posEff + CDbeta * fabs(beta);
	//printDebugMessage("CDF	-->  %f", CDDf * flap_pos);
	//printDebugMessage("CDG	-->  %f", CDGear * gear_pos);
	//printDebugMessage("CDsb	-->  %f", CDsb * sb_pos);
	CYtot = CYbeta * beta + CYDr * def_rudder + CYroll * pp + CYyaw * rr;

	X = qTotS * (-CDtot);
	Y = qTotS * (-CYtot);
	Z = qTotS * CLtot;


	// conversione da assi vento ad assi corpo
	double sa = sin(alpha);
	double ca = cos(alpha);
	double sb = sin(beta);
	double cb = cos(beta);

	double m11 = ca * cb;
	double m12 = sb;
	double m13 = sa * cb;
	double m21 = -ca * sb;
	double m22 = cb;
	double m23 = -sa * sb;
	double m31 = -sa;
	double m32 = 0;
	double m33 = ca;

	resultX = X * m11 + Y * m12 + Z * m13;
	resultY = X * m21 + Y * m22 + Z * m23;
	resultZ = X * m31 + Y * m32 + Z * m33;
	
	// Calcolo coefficienti di momento
	// in questi if/else rendo dinamico il Cmalpha al variare del CG
	double Xcg_empty = -0.351;//-0.551; //-0.191;
	double Cg_pos;
	double CmalphaEff2;

	if (center_of_gravity.x < Xcg_empty)
	{
		Cg_pos = fabs(center_of_gravity.x) - fabs(Xcg_empty);
		CmalphaEff2 = CmalphaEff - Cg_pos;
	}
	else if (center_of_gravity.x > Xcg_empty)
	{
		if (center_of_gravity.x <= 0)
		{
			Cg_pos = fabs(Xcg_empty) - fabs(center_of_gravity.x);
			CmalphaEff2 = CmalphaEff + Cg_pos;
		}
		else
		{
			Cg_pos = fabs(Xcg_empty) + fabs(center_of_gravity.x);
			CmalphaEff2 = CmalphaEff + Cg_pos;
		}
	}
	else
	{
		CmalphaEff2 = CmalphaEff;
	}
	//printDebugMessage("Cmalpha -->  %f", CmalphaEff2);

	Cltot = ClBetaEff * beta + ClDaEff2 * def_aileron + ClDrEff2 * def_rudder + ClRollEff * pp + ClYawEff * rr;

	Cntot = CnBetaEff2 * beta + CnDrEff2 * def_rudder + CnDaEff * def_aileron + CnYawEff * rr + CnRoll * pp;

	Cmtot = Cm0 + CmalphaEff2 * alpha + CmalphaDotEff2 * alphaDot_seg + CmdeEff2 * def_elev + CmqEff2 * qq + CmDf * flap_pos + CmGear * gear_pos + Cmsb * sb_posEff;

	L = qTotSWingSpan * Cltot;
	M = qTotS * MAC * Cmtot;
	N = qTotSWingSpan * Cntot;

	// TODO scrivere codice vero per ECS
	if (quota_slm_m > 6000.0 && quota_slm_m < 10000.0)
	{
		overpressure = 2 * pressione;
	}
	else if (quota_slm_m >= 10000.0)
	{
		overpressure = 3 * pressione;
	}
	else
	{
		overpressure = 0;
	}
	
}

//int AerodyneFM::create_debug_fm()
//{
//	fm_debug = ImGui::ed::reg([this]()
//		{
//			ImGui::Begin("FM Debug");
//			ImGui::SetWindowFontScale(1.0);
//			if (ImGui::CollapsingHeader("Aerodynamics"))
//			{
//				ImGui::Text("Angle Of Attack (AOA): %f", alpha * 180.0 / 3.141592653589793238462643);
//				ImGui::Text("Angle Of Slide (AOS): %f", beta * 180.0 / 3.141592653589793238462643);
//			}
//			if (ImGui::CollapsingHeader("Engine"))
//			{
//				ImGui::Text("Thrust (N): %f", EngineFM.thrust_N);
//				ImGui::Text("NG_RPM: %f", EngineFM.NG_RPM);
//				ImGui::Text("Turbine Gas Temperature (TGT): %f", EngineFM.TGT);
//			}
//			return ImGui::ed::reg_lambda_call_result::IM_ED_USER_CALL_CONTINUE;
//		});
//	return fm_debug;
//}
