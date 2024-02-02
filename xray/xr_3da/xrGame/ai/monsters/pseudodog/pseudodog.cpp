#include "stdafx.h"
#include "pseudodog.h"
#include "pseudodog_state_manager.h"
#include "../../../../skeletonanimated.h"
#include "../../../sound_player.h"
#include "../../../level.h"
#include "../../../ai_monster_space.h"
#include "../monster_velocity_space.h"
#include "../control_animation_base.h"
#include "../control_movement_base.h"

#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "../../../level.h"
#include "../../../../CameraBase.h"
#include "../../../xr_level_controller.h"
#include "../ai_monster_effector.h"

#ifdef _DEBUG
#	include <dinput.h>
#	include "../../../ai_object_location.h"
#	include "../../../level_debug.h"
#	include "../../../level_graph.h"
#	include "../../../ai_space.h"
#	include "../../../alife_simulator.h"
#	include "../../../xrServer_Object_Base.h"
#	include "../../../xrserver.h"
#endif

CAI_PseudoDog::CAI_PseudoDog()
{
	com_man().add_ability(ControlCom::eControlJump);
	com_man().add_ability(ControlCom::eControlRotationJump);
	time_next_psi_attack	= 0;
}

DLL_Pure *CAI_PseudoDog::_construct()
{
	inherited::_construct			();
	StateMan = create_state_manager	();
	return							(this);
}

CAI_PseudoDog::~CAI_PseudoDog()
{
	xr_delete(StateMan);
}

void CAI_PseudoDog::reinit()
{
	inherited::reinit();

	m_time_became_angry				= 0;
	time_growling					= 0;

	time_next_psi_attack	= 0;

	if(CCustomMonster::use_simplified_visual())	return;
	com_man().add_rotation_jump_data	("1","2","3","4", deg(90));
}

void CAI_PseudoDog::Load(LPCSTR section)
{
	inherited::Load	(section);

	anim().AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	anim().AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);
	anim().AddReplacedAnim(&m_bRunTurnLeft,		eAnimRun,		eAnimRunTurnLeft);
	anim().AddReplacedAnim(&m_bRunTurnRight,	eAnimRun,		eAnimRunTurnRight);

	anim().accel_load			(section);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	anim().accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	m_anger_hunger_threshold	= pSettings->r_float(section, "anger_hunger_threshold");
	m_anger_loud_threshold		= pSettings->r_float(section, "anger_loud_threshold");

	// Load psi postprocess --------------------------------------------------------
	LPCSTR ppi_section = pSettings->r_string(section, "psi_effector");
	m_psi_effector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	m_psi_effector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	m_psi_effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	m_psi_effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	m_psi_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	m_psi_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	m_psi_effector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(m_psi_effector.ppi.noise.fps));

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &m_psi_effector.ppi.color_base.r,	&m_psi_effector.ppi.color_base.g,	&m_psi_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &m_psi_effector.ppi.color_gray.r,	&m_psi_effector.ppi.color_gray.g,	&m_psi_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &m_psi_effector.ppi.color_add.r,	&m_psi_effector.ppi.color_add.g,	&m_psi_effector.ppi.color_add.b);

	m_psi_effector.time			= pSettings->r_float(ppi_section,"time");
	m_psi_effector.time_attack	= pSettings->r_float(ppi_section,"time_attack");
	m_psi_effector.time_release	= pSettings->r_float(ppi_section,"time_release");

	m_psi_effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	m_psi_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	m_psi_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	m_psi_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	// --------------------------------------------------------------------------------
	psy_effect_turn_angle		= angle_normalize(pSettings->r_float(section,"psy_effect_turn_angle"));

	::Sound->create(psy_effect_sound,pSettings->r_string(section,"sound_psy_effect"),st_Effect,SOUND_TYPE_WORLD);

	SVelocityParam &velocity_none		= move().get_velocity(MonsterMovement::eVelocityParameterIdle);	
	SVelocityParam &velocity_turn		= move().get_velocity(MonsterMovement::eVelocityParameterStand);
	SVelocityParam &velocity_walk		= move().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam &velocity_run		= move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	SVelocityParam &velocity_walk_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
	SVelocityParam &velocity_run_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
	SVelocityParam &velocity_steal		= move().get_velocity(MonsterMovement::eVelocityParameterSteal);
	SVelocityParam &velocity_drag		= move().get_velocity(MonsterMovement::eVelocityParameterDrag);


	// define animation set
	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimEat,			"stand_eat_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimSleep,			"lie_sleep_",			-1, &velocity_none,		PS_LIE);
	anim().AddAnim(eAnimLieIdle,		"lie_idle_",			-1, &velocity_none,		PS_LIE);
	anim().AddAnim(eAnimSitIdle,		"sit_idle_",			-1, &velocity_none,		PS_SIT);
	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimWalkFwd,		"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND);
	anim().AddAnim(eAnimWalkDamaged,	"stand_walk_dmg_",		-1, &velocity_walk_dmg,	PS_STAND);
	anim().AddAnim(eAnimRun,			"stand_run_",			-1,	&velocity_run,		PS_STAND);
	anim().AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &velocity_run_dmg,	PS_STAND);
	anim().AddAnim(eAnimCheckCorpse,	"stand_check_corpse_",	-1,	&velocity_none,		PS_STAND);
	anim().AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &velocity_drag,		PS_STAND);
	anim().AddAnim(eAnimSniff,			"stand_sniff_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimHowling,		"stand_howling_",		-1,	&velocity_none,		PS_STAND);
	anim().AddAnim(eAnimJumpGlide,		"jump_glide_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,	PS_STAND);
	anim().AddAnim(eAnimDie,			"stand_die_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, &velocity_none,		PS_SIT);
	anim().AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &velocity_none,		PS_STAND);	
	anim().AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &velocity_none,		PS_SIT);
	anim().AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1,	&velocity_none,		PS_LIE);
	anim().AddAnim(eAnimSleepStandUp,	"lie_to_stand_up_",		-1, &velocity_none,		PS_LIE);
	anim().AddAnim(eAnimAttackPsi,		"stand_psi_attack_",	-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimThreaten,		"stand_howling_",		-1,	&velocity_none,		PS_STAND);
	
	anim().AddAnim(eAnimRunTurnLeft,	"stand_run_turn_left_",	-1, &velocity_run,		PS_STAND);
	anim().AddAnim(eAnimRunTurnRight,	"stand_run_turn_right_",-1, &velocity_run,		PS_STAND);


	// define transitions
	// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
	anim().AddTransition(eAnimLieIdle,	eAnimSleep,	eAnimLieToSleep,	false);
	anim().AddTransition(eAnimSleep,	PS_STAND,	eAnimSleepStandUp,	false);
	anim().AddTransition(PS_SIT,		PS_LIE,		eAnimSitLieDown,	false);
	anim().AddTransition(PS_STAND,		PS_SIT,		eAnimStandSitDown,	false);
	anim().AddTransition(PS_SIT,		PS_STAND,	eAnimSitStandUp,	false);

	// define links from Action to animations
	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,	eAnimWalkBkwd);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,		eAnimSleep);
	anim().LinkAction(ACT_REST,			eAnimSitIdle);
	anim().LinkAction(ACT_DRAG,			eAnimDragCorpse);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,		eAnimWalkFwd);	
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimSniff);


#ifdef DEBUG	
	anim().accel_chain_test		();
#endif

}


void CAI_PseudoDog::reload(LPCSTR section)
{
	inherited::reload			(section);

	if(CCustomMonster::use_simplified_visual())	return;
	
	// load additional sounds
	sound().add					(pSettings->r_string(section,"sound_psy_attack"), DEFAULT_SAMPLE_COUNT,	SOUND_TYPE_MONSTER_ATTACKING,	MonsterSound::eHighPriority+3,	MonsterSound::eBaseChannel,	ePsyAttack, "bip01_head");
	
	// load jump params
	com_man().load_jump_data			(0,"run_jamp_0", "run_jamp_1", "run_jamp_2", MonsterMovement::eVelocityParameterRunNormal,MonsterMovement::eVelocityParameterRunNormal,0);

}

// постоянный быстрый апдейт
void CAI_PseudoDog::CheckSpecParams(u32 spec_params)
{

	if ((spec_params & ASP_PSI_ATTACK) == ASP_PSI_ATTACK) {
		com_man().seq_run(anim().get_motion_id(eAnimAttackPsi));
	}

	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		anim().SetCurAnim(eAnimThreaten);
	}
}


void CAI_PseudoDog::UpdateCL()
{
	inherited::UpdateCL();

	
	if (time_next_psi_attack > m_dwCurrentTime) return;
	time_next_psi_attack			= m_dwCurrentTime + Random.randI(4000,6000);
	if (!g_Alive()) return;
	if (!EnemyMan.get_enemy()) return;
	if (!EnemyMan.see_enemy_now()) return;
	CActor *pA = const_cast<CActor *>(smart_cast<const CActor *>(EnemyMan.get_enemy()));  // possibpy get_enemy error (fixed? 21.04.18)
	if (!pA) return;
	float dist_to_actor = pA->Position().distance_to(Position()); 
	if (dist_to_actor <= 10.0f) {
	com_man().seq_run(anim().get_motion_id(eAnimAttackPsi));
	play_effect_sound();
	pA->Cameras().AddCamEffector(xr_new<CMonsterEffectorHit>(m_psi_effector.ce_time,m_psi_effector.ce_amplitude,m_psi_effector.ce_period_number,m_psi_effector.ce_power));
	pA->Cameras().AddPPEffector(xr_new<CMonsterEffector>(m_psi_effector.ppi, m_psi_effector.time, m_psi_effector.time_attack, m_psi_effector.time_release));
		if (pA->cam_Active()) {
				pA->cam_Active()->Move(Random.randI(2) ? kRIGHT : kLEFT, Random.randF(psy_effect_turn_angle)); 
				pA->cam_Active()->Move(Random.randI(2) ? kUP	: kDOWN, Random.randF(psy_effect_turn_angle)); 
		}
	}
			//Msg("timer psy alive");
	
	


	//if (time_next_psi_attack > m_dwCurrentTime) return;
	//time_next_psi_attack			= m_dwCurrentTime + Random.randI(4000,6000);
	//// start postprocess
	//if (EnemyMan.get_enemy()){
	//auto pA = const_cast<CActor *>(smart_cast<const CActor *>(EnemyMan.get_enemy()));  // possibpy get_enemy error (fixed? 21.04.18)
	////CActor *pA = const_cast<CActor *>(smart_cast<const CActor *>(EnemyMan.get_enemy()));  // possibpy get_enemy error (fixed? 21.04.18) // рабочий
	////auto pA = smart_cast<CGameObject*>(g_actor);
	//if (!pA) return;
	//if (g_Alive()) {
	//if (Position().distance_to(pA->Position()) <= 10.0f) {
	//com_man().seq_run(anim().get_motion_id(eAnimAttackPsi));
	//play_effect_sound();
	////CActor* pA =  smart_cast<CActor*>(Level().CurrentEntity());
	//	//if (pA) {
	//		pA->Cameras().AddCamEffector(xr_new<CMonsterEffectorHit>(m_psi_effector.ce_time,m_psi_effector.ce_amplitude,m_psi_effector.ce_period_number,m_psi_effector.ce_power));
	//		pA->Cameras().AddPPEffector(xr_new<CMonsterEffector>(m_psi_effector.ppi, m_psi_effector.time, m_psi_effector.time_attack, m_psi_effector.time_release));

	//		if (pA->cam_Active()) {
	//			pA->cam_Active()->Move(Random.randI(2) ? kRIGHT : kLEFT, Random.randF(psy_effect_turn_angle)); 
	//			pA->cam_Active()->Move(Random.randI(2) ? kUP	: kDOWN, Random.randF(psy_effect_turn_angle)); 
	//		//}
	//	}
	//}
	//		//Msg("timer psy alive");
	//}
	//}

}

void CAI_PseudoDog::HitEntityInJump		(const CEntity *pEntity) 
{
	SAAParam &params	= anim().AA_GetParams("run_jamp_1");
	HitEntity			(pEntity, params.hit_power, params.impulse, params.impulse_dir);
}


#ifdef _DEBUG
void CAI_PseudoDog::debug_on_key(int key)
{
}
#endif

IStateManagerBase *CAI_PseudoDog::create_state_manager()
{
	return xr_new<CStateManagerPseudodog>(this);
}


void CAI_PseudoDog::play_effect_sound()
{
	CActor *pA = smart_cast<CActor*>(Level().CurrentEntity());
	if (!pA) return;
	//Msg("play psy sound");
	Fvector pos = pA->Position();
	pos.y += 1.5f;
	::Sound->play_at_pos(psy_effect_sound,pA,pos);
}
