
//
//void CAI_Chimera::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
//{
//	if (bShowDeath)	{
//		m_tpCurrentGlobalAnimation = m_tAnimations.A[0].A[0].A[0];
//		PKinematics(Visual())->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
//		bShowDeath  = false;
//		return;
//	}
//	
//	if (g_Alive())
//		if (!m_tpCurrentGlobalAnimation) {
//
//			int i1, i2, i3;
//			i1 = i2 = i3 = 0;
//			MotionToAnim(m_tAnim,i1,i2,i3);
//			if (i3 == -1) {
//				i3 = ::Random.randI((int)m_tAnimations.A[i1].A[i2].A.size());
//			}
//		
//			m_tpCurrentGlobalAnimation = m_tAnimations.A[i1].A[i2].A[i3];
//			PKinematics(Visual())->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
//
//			if (i2 == 9) FillAttackStructure(i3, m_dwCurrentUpdate);
////			else if (i2 == 10) FillAttackStructure(4, m_dwCurrentUpdate); // атака крыс
////			else if (i2 == 19) FillAttackStructure(5, m_dwCurrentUpdate); // атака крыс|прыжок
//			else m_tAttack.time_started = 0;
//		}
//}
//
